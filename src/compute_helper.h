#include <SDL.h>
#include <complex.h> /* Standard Library of Complex Numbers */

#define WIN_WIDTH 600
#define WIN_HEIGHT 600

#define MAX_ITERATIONS 500
#define NUM_THREADS 100

typedef struct {
  int iterations;
  double distance;
} mandelbrot_escape_data;

typedef struct {
  int x;
  int y;
} coordinate;

typedef struct {
  uint32_t R;
  uint32_t G;
  uint32_t B;
} RGB_Color;

int coordinate_to_index(coordinate c);
coordinate index_to_coordinate(int index);
double convert_ranges(double oldValue, double oldMin, double oldMax,
                      double newMin, double newMax);
mandelbrot_escape_data get_mandelbrot_iterations(double x, double y,
                                                 int max_iterations);
RGB_Color hsv_to_rgb(float H, float S, float V);
