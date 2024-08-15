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
double zoom = 1;
double zoom_change = 5;
double max_iterations = 250;
double iteration_change = 20;

int coordinate_to_index(coordinate c);
coordinate index_to_coordinate(int index);
double convert_ranges(double oldValue, double oldMin, double oldMax, double newMin, double newMax);
int get_mandelbrot_iterations(double x, double y);
void render_fractal(SDL_Texture *texture, double min_x, double max_x, double min_y, double max_y);

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

	//complex plane viewport
	double min_c_x = -2.5;
	double max_c_x = 1.5;
	double min_c_y = -2;
	double max_c_y = 2;
	render_fractal(texture, min_c_x, max_c_x, min_c_y, max_c_y);


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
				render_fractal(texture, min_c_x, max_c_x, min_c_y, max_c_y);
				zoom/=zoom_change;
				max_iterations += iteration_change;
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
void render_fractal(SDL_Texture *texture, double min_x, double max_x, double min_y, double max_y){
    // array of pixel
    uint8_t pixels[WIN_WIDTH * WIN_HEIGHT * 4] = {0};

    for (int i=0; i<WIN_HEIGHT*WIN_WIDTH*4; i+=4) {
		coordinate c = index_to_coordinate(i);
		double x_new = convert_ranges(c.x, 0, WIN_WIDTH, min_x, max_x);
		double y_new = convert_ranges(c.y, 0, WIN_HEIGHT, min_y, max_y);
		int num_iter = get_mandelbrot_iterations(x_new, y_new);
		if(num_iter < max_iterations){
			pixels[i] = num_iter; //R
			pixels[i+1] = num_iter; //G
			pixels[i+2] = num_iter; //B
			pixels[i+3] = 255; //alpha
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
}

