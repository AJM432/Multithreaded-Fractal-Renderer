#include <complex.h> /* Standard Library of Complex Numbers */

#define WIN_WIDTH 600
#define WIN_HEIGHT 600

#define MAX_ITERATIONS 100
#define NUM_THREADS 100

typedef struct coord {
  int x;
  int y;
} coordinate;

int coordinate_to_index(coordinate c);
coordinate index_to_coordinate(int index);
double convert_ranges(double oldValue, double oldMin, double oldMax,
                      double newMin, double newMax);
int get_mandelbrot_iterations(double x, double y, int max_iterations);
