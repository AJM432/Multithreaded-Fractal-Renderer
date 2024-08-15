#include <stdio.h>
#include <stdbool.h>
#include <complex.h>    /* Standard Library of Complex Numbers */
#include <pthread.h>
#include "queue.h"
/* #include <string.h> */

#include <SDL.h>

#define WIN_WIDTH 600
#define WIN_HEIGHT 600
#define MAX_ITERATIONS 1000
#define NUM_THREADS 100

typedef struct coord{
	int x;
	int y;
} coordinate;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
node_t *g_worker_queue = NULL;
int g_processes_left = WIN_HEIGHT;
double zoom = 1;
double zoom_change = 5;
double max_iterations = MAX_ITERATIONS;
double iteration_change = 20;
//
// array of pixel
uint32_t pixels[WIN_WIDTH * WIN_HEIGHT] = {0};

//complex plane viewport
double min_c_x = -2.5;
double max_c_x = 1.5;
double min_c_y = -2;
double max_c_y = 2;
SDL_Texture *texture;
SDL_Renderer *renderer;


int coordinate_to_index(coordinate c);
coordinate index_to_coordinate(int index);
double convert_ranges(double oldValue, double oldMin, double oldMax, double newMin, double newMax);
int get_mandelbrot_iterations(double x, double y);
void render_fractal(SDL_Texture *texture);
int safe_check_processes_left();
void *thread_function(void *);


int main(int argc, char **argv) {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // create SDL window
    SDL_Window *window = SDL_CreateWindow("Multi-threaded Fractal Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return 1;
    }

    // create renderer
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        return 1;
    }
    
    SDL_RenderSetLogicalSize(renderer, WIN_WIDTH, WIN_HEIGHT);

    // create texture
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WIN_WIDTH,
        WIN_HEIGHT);
    if (texture == NULL) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return 1;
    }

	render_fractal(texture);


    // main loop
    bool should_quit = false;
    SDL_Event e;
    while (!should_quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                should_quit = true;
            }
			else if ( e.type == SDL_MOUSEBUTTONDOWN) {
				/* printf("Mouse moved to (%d,%d)\n", e.button.x, e.button.y); */
				double x = convert_ranges(e.button.x, 0, WIN_WIDTH, min_c_x, max_c_x);
				double y = convert_ranges(e.button.y, 0, WIN_HEIGHT, min_c_y, max_c_y);

				min_c_x = x-zoom;
				min_c_y = y-zoom;
				max_c_x = x+zoom;
				max_c_y = y+zoom;
				render_fractal(texture);
				if ( e.button.button == SDL_BUTTON_LEFT) {
					zoom/=zoom_change;
					max_iterations += iteration_change;
				}
				else if ( e.button.button == SDL_BUTTON_RIGHT) {
					zoom*=zoom_change;
					max_iterations -= iteration_change;
				}
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

int coordinate_to_index(coordinate c){
	return (c.y*WIN_HEIGHT + c.x);
}

coordinate index_to_coordinate(int index) {
	coordinate c;
	c.x = (index)%WIN_WIDTH;
	c.y = (index)/WIN_HEIGHT;
	return c;
}

double convert_ranges(double oldValue, double oldMin, double oldMax, double newMin, double newMax) {
	return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;
}

int get_mandelbrot_iterations(double x, double y) {
	double complex z = 0;
	double complex c = x+y*I;


	for(int i=0; i < max_iterations; i++){
		z =z*z+c;
		if(cabs(z) >= 2){
			return i;
		}
	}
	return max_iterations; // in set (black)
}
void render_fractal(SDL_Texture *texture){

	// init all items to process in worker queue
    /* while (dequeue(&g_worker_queue) > 0);// dequeue all items first */
	for(int i=0; i < WIN_HEIGHT; i++){
		enqueue(&g_worker_queue, i);
	}

	g_processes_left = WIN_HEIGHT;
	pthread_t thread_id[NUM_THREADS];
	for(int i=0; i < NUM_THREADS; i++){
		pthread_create( &thread_id[i], NULL, thread_function, NULL );
	}

	for(int i=0; i < NUM_THREADS; i++){
		pthread_join( thread_id[i], NULL ); 
	}
	
    /* for (int i=0; i<WIN_HEIGHT*WIN_WIDTH; i++) { */
		/* coordinate c = index_to_coordinate(i); */
		/* double x_new = convert_ranges(c.x, 0, WIN_WIDTH, min_x, max_x); */
		/* double y_new = convert_ranges(c.y, 0, WIN_HEIGHT, min_y, max_y); */
		/* int num_iter = get_mandelbrot_iterations(x_new, y_new); */
		/* if(num_iter < max_iterations){ */
			/* num_iter = (uint8_t)convert_ranges(num_iter, 0, max_iterations, 0, 255); */
			/* /1* pixels[i] = (ALPHA << 24) | (BLUE << 16) | (GREEN << 8) | (RED << 0); *1/ */
			/* /1* pixels[i] = (255 << 24) | (num_iter << 16) | (num_iter << 8) | (num_iter << 0); *1/ */
			/* pixels[i] = (255 << 24) | (0 << 16) | (num_iter << 8) | (0 << 0); */
		/* } */
		/* else{ */
			/* pixels[i] = 0; */
		/* } */
    /* } */

    // update texture with new data
    int texture_pitch = 0;
    void* texture_pixels = NULL;
    if (SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch) != 0) {
        SDL_Log("Unable to lock texture: %s", SDL_GetError());
    }
    else {
        memcpy(texture_pixels, pixels, texture_pitch * WIN_HEIGHT);
    }
    SDL_UnlockTexture(texture);
}

void *thread_function(void *ptr){
	int line_task;
	uint32_t line[WIN_WIDTH] = {};
	while(1){
		pthread_mutex_lock(&mutex);
		line_task=dequeue(&g_worker_queue);
		pthread_mutex_unlock(&mutex);
		if (line_task == -1){
			break;
		}
		for (int i=0; i<WIN_WIDTH; i++) {
			coordinate c = index_to_coordinate(line_task*WIN_WIDTH + i);
			double x_new = convert_ranges(c.x, 0, WIN_WIDTH, min_c_x, max_c_x);
			double y_new = convert_ranges(c.y, 0, WIN_HEIGHT, min_c_y, max_c_y);
			int num_iter = get_mandelbrot_iterations(x_new, y_new);
			if(num_iter < max_iterations){
				num_iter = (uint8_t)convert_ranges(num_iter, 0, max_iterations, 0, 255);
				/* line[i] = (ALPHA << 24) | (BLUE << 16) | (GREEN << 8) | (RED << 0); */
				/* line[i] = (255 << 24) | (num_iter << 16) | (num_iter << 8) | (num_iter << 0); */
				line[i] = (255 << 24) | (0 << 16) | (num_iter << 8) | (0 << 0);
			}
			else{//TODO: why can't I just zero the array first?
				line[i] = 0;
			}
		}

	pthread_mutex_lock(&mutex);
	memcpy(pixels + WIN_WIDTH*line_task, line, sizeof(uint32_t)*WIN_WIDTH);
    pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

int safe_check_processes_left(){
    int status;
    int result;

    pthread_mutex_lock(&mutex);
    result = g_processes_left;
    pthread_mutex_unlock(&mutex);
    return result;
}
