<div align="center">
<img style="border-radius: 10px" width="863" src="https://github.com/user-attachments/assets/90330074-71fb-4517-812c-92571b57495b">
</div>

# Multithreaded Fractal Renderer

Fast multithreaded fractal renderer written in C.

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)

## Demo
<div align="center">
  <img width="600" alt="screen_shot" src="https://github.com/user-attachments/assets/101b3e03-38c1-4641-82c8-39fadd903530">
</div>

## Implementation
In progress ⚙️...

## Features
- Use the mouse cursor to zoom in and out using left and right mouse button presses
- Use the keyboard to navigate as well. Use the plus/minus keys to zoom in/out and arrow keys to pan an area

## Usage/Examples
Change the iteration formula to see different fractals, currently set to $$z = z^2 + C$$ which renders the Mandelbrot set.

```c
mandelbrot_escape_data get_mandelbrot_iterations(double x, double y,
                                                 int max_iterations) {
  double complex z = 0;
  double complex c = x + y * I;
  for (int i = 0; i < max_iterations; i++) {
    z = z * z + c; //FORMULA
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

```

## Install & Run
Ensure you have C, and make installed.

Clone the project

```bash
git clone https://github.com/Multithreaded-Fractal-Renderer.git

```


Go to the project directory

```bash
$ cd Multithreaded-Fractal-Renderer
```

Build and run the project

```bash
$ make
$ ./build/fractal
```

## Feedback

If you have any feedback, please reach out to me at alvinjosematthew@gmail.com

## License

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
## 🔗 Links
[![portfolio](https://img.shields.io/badge/my_portfolio-000?style=for-the-badge&logo=ko-fi&logoColor=white)](https://alvinmatthew.com/)
