#include <cmath>

#include "canvas.h"
#include "open_gl.h"

#include "src/voronoi.h"

const int WIDTH = 500;
const int HEIGHT = 300;
const float PIXEL_SIZE = 3;

Canvas canvas = Canvas(WIDTH, HEIGHT);
OpenGL openGL = OpenGL(&canvas, PIXEL_SIZE, WINDOWED_RESIZABLE);
Voronoi voronoi = Voronoi(WIDTH, HEIGHT, 10);

int main()
{
    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
    });

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        voronoi.draw(&canvas);

        openGL.update(&canvas);
    }

    return EXIT_SUCCESS;
}
