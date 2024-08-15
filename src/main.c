#include <stdio.h>
#include <stdbool.h>
#include <complex.h>    /* Standard Library of Complex Numbers */

#include <SDL.h>

#define WIN_WIDTH 600
#define WIN_HEIGHT 600
#define MAX_ITERATIONS 250

struct coordinate{
	int x;
	int y;
};
typedef struct coordinate coordinate;

int coordinate_to_index(coordinate c);
coordinate index_to_coordinate(int index);
double convert_ranges(double oldValue, double oldMin, double oldMax, double newMin, double newMax);
int in_mandelbrot(double x, double y);

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
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        return 1;
    }
    
    SDL_RenderSetLogicalSize(renderer, WIN_WIDTH, WIN_HEIGHT);

    // create texture
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WIN_WIDTH,
        WIN_HEIGHT);
    if (texture == NULL) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return 1;
    }

    // array of pixels
    uint8_t pixels[WIN_WIDTH * WIN_HEIGHT * 4] = {0};

    for (int i=0; i<WIN_HEIGHT*WIN_WIDTH*4; i+=4) {
		coordinate c = index_to_coordinate(i);
		double x_new = convert_ranges(c.x, 0, WIN_WIDTH, -2.5, 1.5);
		double y_new = convert_ranges(c.y, 0, WIN_HEIGHT, -2, 2);
		if(!in_mandelbrot(x_new, y_new)){
			pixels[i] = 255;
			pixels[i+1] = 255;
			pixels[i+2] = 255;
			pixels[i+3] = 255;
		}
    }

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

    // main loop
    bool should_quit = false;
    SDL_Event e;
    while (!should_quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                should_quit = true;
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
	return 4*(c.y*WIN_HEIGHT + c.x);
}

coordinate index_to_coordinate(int index) {
	coordinate c;
	c.x = (index/4)%WIN_WIDTH;
	c.y = (index/4)/WIN_HEIGHT;
	return c;
}

double convert_ranges(double oldValue, double oldMin, double oldMax, double newMin, double newMax) {
	return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;

  /* double oldRange = (oldMax - oldMin); */
  /* double newValue; */
  /* if (oldRange == 0) */
  /*     newValue = newMin; */
  /* else */
  /* { */
  /*     double newRange = (newMax - newMin); */
  /*     newValue = (((oldValue - oldMin) * newRange) / oldRange) + newMin; */
  /* } */
  /* return newValue; */
}

int in_mandelbrot(double x, double y) {
	double complex z = 0;
	double complex c = x+y*I;


	for(int i=0; i < MAX_ITERATIONS; i++){
		z =z*z+c;
		if(cabs(z) >= 2){
			return 0;
		}
	}
	return 1;
}
