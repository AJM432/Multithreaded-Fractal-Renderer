#include "compute_helper.h"
#include <math.h>

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

mandelbrot_escape_data get_mandelbrot_iterations(double x, double y,
                                                 int max_iterations) {
  double complex z = 0;
  double complex c = x + y * I;
  for (int i = 0; i < max_iterations; i++) {
    z = z * z + c;
    double dist = cabs(z);
    if (dist >= 2) {
      mandelbrot_escape_data data = {.iterations = i, .distance = dist};
      return data;
    }
  }
  mandelbrot_escape_data data = {.iterations = max_iterations,
                                 .distance = cabs(z)};
  return data; // in set (black)
}

RGB_Color hsv_to_rgb(double H, double S, double V) {
  float r, g, b;

  double h = H / 360.0;
  double s = S / 100.0;
  double v = V / 100.0;

  int i = floor(h * 6);
  double f = h * 6 - i;
  double p = v * (1 - s);
  double q = v * (1 - f * s);
  double t = v * (1 - (1 - f) * s);

  switch (i % 6) {
  case 0:
    r = v, g = t, b = p;
    break;
  case 1:
    r = q, g = v, b = p;
    break;
  case 2:
    r = p, g = v, b = t;
    break;
  case 3:
    r = p, g = q, b = v;
    break;
  case 4:
    r = t, g = p, b = v;
    break;
  case 5:
    r = v, g = p, b = q;
    break;
  }

  RGB_Color c;
  c.R = (uint32_t)r * 255;
  c.G = (uint32_t)g * 255;
  c.B = (uint32_t)b * 255;

  return c;
}

double *linspace(double x1, double x2, int n) {

  double *x = calloc(n, sizeof(double));

  double step = (x2 - x1) / (double)(n - 1);

  for (int i = 0; i < n; i++) {
    x[i] = x1 + ((double)i * step);
  }

  return x;
}
