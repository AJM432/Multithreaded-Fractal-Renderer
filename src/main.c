#include "compute_helper.h"
#include "queue.h"
#include <SDL.h>
#include <pthread.h>
#include <stdio.h>

// global variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double zoom = 1;
double zoom_change = 3;
double max_iterations = MAX_ITERATIONS;
double iteration_change = 0;

// array of pixel data
uint32_t pixels[WIN_WIDTH * WIN_HEIGHT] = {0};
// global worker queue
node_t *g_worker_queue = NULL;

// complex plane viewport
double min_c_x = -2;
double max_c_x = 2;
double min_c_y = -2;
double max_c_y = 2;

// function definitions
void render_fractal(SDL_Texture *texture);
void *thread_worker(void *);

RGB_Color get_smooth_color(int iterations, double escape_radius,
                           double max_iterations) {
  double mu = iterations - log(log(escape_radius)) / log(2.0);
  double normalized = mu / max_iterations;
  double hue = 360.0 * normalized;

  // Convert HSV to RGB
  return hsv_to_rgb(hue, 100.0, 100.0);
}

int main(int argc, char **argv) {
  // SDL init
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  // create SDL window
  SDL_Window *window = SDL_CreateWindow(
      "Multi-threaded Fractal Renderer", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    SDL_Log("Unable to create window: %s", SDL_GetError());
    return 1;
  }

  // create renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    SDL_Log("Unable to create renderer: %s", SDL_GetError());
    return 1;
  }

  SDL_RenderSetLogicalSize(renderer, WIN_WIDTH, WIN_HEIGHT);

  // create texture
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                        SDL_TEXTUREACCESS_STREAMING, WIN_WIDTH, WIN_HEIGHT);
  if (texture == NULL) {
    SDL_Log("Unable to create texture: %s", SDL_GetError());
    return 1;
  }

  render_fractal(texture);

  // main loop
  int should_quit = 0;
  SDL_Event e;
  while (!should_quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        should_quit = 1;
      } else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN) {
        int screen_x, screen_y;

        if (e.key.keysym.sym == SDLK_EQUALS || e.key.keysym.sym == SDLK_MINUS) {
          screen_x = WIN_WIDTH / 2;
          screen_y = WIN_HEIGHT / 2;
        }

        else if (e.key.keysym.sym == SDLK_RIGHT) {
          screen_x = WIN_WIDTH;
          screen_y = WIN_HEIGHT / 2;
        } else if (e.key.keysym.sym == SDLK_LEFT) {
          screen_x = 0;
          screen_y = WIN_HEIGHT / 2;
        } else if (e.key.keysym.sym == SDLK_UP) {
          screen_x = WIN_WIDTH / 2;
          screen_y = 0;
        } else if (e.key.keysym.sym == SDLK_DOWN) {
          screen_x = WIN_WIDTH / 2;
          screen_y = WIN_HEIGHT;
        }

        else if (e.type == SDL_MOUSEBUTTONDOWN) {
          screen_x = e.button.x;
          screen_y = e.button.y;
        } else {
          continue;
        }

        if (e.button.button == SDL_BUTTON_LEFT ||
            e.key.keysym.sym == SDLK_EQUALS) {
          zoom /= zoom_change;
          max_iterations += iteration_change;
        } else if (e.button.button == SDL_BUTTON_RIGHT ||
                   e.key.keysym.sym == SDLK_MINUS) {
          zoom *= zoom_change;
          max_iterations -= iteration_change;
        }

        /* printf("Mouse moved to (%d,%d)\n", e.button.x, e.button.y); */
        double x = convert_ranges(screen_x, 0, WIN_WIDTH, min_c_x, max_c_x);
        double y = convert_ranges(screen_y, 0, WIN_HEIGHT, min_c_y, max_c_y);

        min_c_x = x - zoom;
        min_c_y = y - zoom;
        max_c_x = x + zoom;
        max_c_y = y + zoom;
        render_fractal(texture);
      }
    }

    // render on screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void render_fractal(SDL_Texture *texture) {

  // init all items to process in worker queue
  for (int i = 0; i < WIN_HEIGHT; i++) {
    enqueue(&g_worker_queue, i);
  }

  pthread_t thread_id[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&thread_id[i], NULL, thread_worker, NULL);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(thread_id[i], NULL);
  }

  // update texture with new data
  int texture_pitch = 0;
  void *texture_pixels = NULL;
  if (SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch) != 0) {
    SDL_Log("Unable to lock texture: %s", SDL_GetError());
  } else {
    memcpy(texture_pixels, pixels, texture_pitch * WIN_HEIGHT);
  }
  SDL_UnlockTexture(texture);
}

void *thread_worker(void *ptr) {
  int line_task;
  uint32_t line[WIN_WIDTH] = {}; // pixel data for a single line
  while (1) {
    pthread_mutex_lock(&mutex);
    line_task = dequeue(&g_worker_queue);
    pthread_mutex_unlock(&mutex);
    if (line_task == -1) { // no more items in the queue
      break;
    }

    double y_new = convert_ranges(index_to_coordinate(line_task * WIN_WIDTH).y,
                                  0, WIN_HEIGHT, min_c_y, max_c_y);
    double *x_new_array = linspace(min_c_x, max_c_x, WIN_WIDTH);
    for (int i = 0; i < WIN_WIDTH; i++) {
      coordinate c = index_to_coordinate(line_task * WIN_WIDTH + i);
      mandelbrot_escape_data data =
          get_mandelbrot_iterations(x_new_array[i], y_new, max_iterations);
      if (data.iterations < max_iterations) {
        // line[i] = (ALPHA << 24) | (BLUE << 16) | (GREEN << 8) | (RED << 0);
        RGB_Color color =
            get_smooth_color(data.iterations, data.distance, max_iterations);
        line[i] =
            (255 << 24) | (color.B << 16) | (color.G << 8) | (color.R << 0);
      } else {
        line[i] = 0;
      }
    }
    free(x_new_array);

    pthread_mutex_lock(&mutex);
    memcpy(pixels + WIN_WIDTH * line_task, line, sizeof(uint32_t) * WIN_WIDTH);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}
