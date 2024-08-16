#include "compute_helper.h"

int coordinate_to_index(coordinate c) { return (c.y * WIN_HEIGHT + c.x); }

coordinate index_to_coordinate(int index) {
  coordinate c;
  c.x = (index) % WIN_WIDTH;
  c.y = (index) / WIN_HEIGHT;
  return c;
}

double convert_ranges(double oldValue, double oldMin, double oldMax,
                      double newMin, double newMax) {
  return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) +
         newMin;
}

int get_mandelbrot_iterations(double x, double y, int max_iterations) {
  double complex z = 0;
  double complex c = x + y * I;
  for (int i = 0; i < max_iterations; i++) {
    z = z * z + c;
    if (cabs(z) >= 2) {
      return i;
    }
  }
  return max_iterations; // in set (black)

  /* double a, b; */
  /* int i; */
  /* for(i=0; (i < max_iterations) && (a*a+b*b <= 4); i++){ */
  /* 	double tmp = a*a-b*b + x; */
  /* 	b = 2*a*b + y; */
  /* 	a = tmp; */
  /* } */
  /* return i; */
}
