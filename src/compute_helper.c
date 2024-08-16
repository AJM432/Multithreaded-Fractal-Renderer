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

  /* double a, b; */
  /* int i; */
  /* for(i=0; (i < max_iterations) && (a*a+b*b <= 4); i++){ */
  /* 	double tmp = a*a-b*b + x; */
  /* 	b = 2*a*b + y; */
  /* 	a = tmp; */
  /* } */
  /* return i; */
}
RGB_Color hsv_to_rgb(float H, float S, float V) {
  float r, g, b;

  float h = H / 360;
  float s = S / 100;
  float v = V / 100;

  int i = floor(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

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
