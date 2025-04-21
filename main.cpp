#include <cmath>
#include <random>
#include <iostream>

#include "canvas.h"
#include "open_gl.h"
#include "mouse.h"

#include "src/site.h"
#include "src/voronoi.h"

const int WIDTH = 256;
const int HEIGHT = 256;
const float PIXEL_SIZE = 3;

Canvas canvas = Canvas(WIDTH, HEIGHT);
OpenGL openGL = OpenGL(&canvas, PIXEL_SIZE, WINDOWED_RESIZABLE);
Mouse mouse = Mouse(&openGL, 1, MOUSE_CURSOR_ENABLED);

Site* setRandomSites(int count)
{
    Site* sites = new Site[count];
    for (int i = 0; i < count; i++) {
        sites[i].x = rand() % WIDTH;
        sites[i].y = rand() % HEIGHT;
    }
    return sites;
}

int main()
{
    std::string seed = "1745185802";//std::to_string(time(NULL));

    srand(std::hash<std::string>()(seed));
    std::cout << "Seed: " << seed << std::endl;

    int siteCount = 3;
    Site* sites = setRandomSites(siteCount);
    Voronoi voronoi = Voronoi(WIDTH, HEIGHT, siteCount, sites);

    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
    });

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        mouse.update(&openGL);

        // if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT)) {
        //     voronoi.setSite(mouse.xPos / PIXEL_SIZE, canvas.height - 1 - mouse.yPos / PIXEL_SIZE);
        // }

        voronoi.show(&canvas, canvas.height - 1 -mouse.yPos / PIXEL_SIZE);

        openGL.update(&canvas);
    }

    std::cout << "Seed: " << seed << std::endl;

    return EXIT_SUCCESS;
}
