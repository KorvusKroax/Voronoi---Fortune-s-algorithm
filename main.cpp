#include <cmath>

#include "canvas.h"
#include "open_gl.h"
#include "mouse.h"

#include "src/voronoi.h"

const int WIDTH = 500;
const int HEIGHT = 300;
const float PIXEL_SIZE = 3;

Canvas canvas = Canvas(WIDTH, HEIGHT);
OpenGL openGL = OpenGL(&canvas, PIXEL_SIZE, WINDOWED_RESIZABLE);
Mouse mouse = Mouse(&openGL, 1, MOUSE_CURSOR_ENABLED);

Voronoi voronoi = Voronoi(WIDTH, HEIGHT, 10);

int main()
{
    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
    });

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        mouse.update(&openGL);

        if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT)) {
            voronoi.setSite(mouse.xPos / PIXEL_SIZE, canvas.height - 1 - mouse.yPos / PIXEL_SIZE);
        }
        voronoi.draw(&canvas, mouse.xPos / PIXEL_SIZE, canvas.height - 1 -mouse.yPos / PIXEL_SIZE);

        openGL.update(&canvas);
    }

    return EXIT_SUCCESS;
}
