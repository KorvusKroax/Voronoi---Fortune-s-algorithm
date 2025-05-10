#include <cmath>
#include <random>
#include <iostream>

#include "canvas.h"
#include "open_gl.h"
#include "mouse.h"
#include "line.h"
#include "circle.h"

#include "src/site.h"
#include "src/voronoi.h"

const int WIDTH = 256;
const int HEIGHT = 256;
const float PIXEL_SIZE = 3;

Canvas canvas = Canvas(WIDTH, HEIGHT);
OpenGL openGL = OpenGL(&canvas, PIXEL_SIZE, WINDOWED_RESIZABLE);
Mouse mouse = Mouse(&openGL, 1, MOUSE_CURSOR_ENABLED);

Site* sites;
int siteCount = 5;
int siteIndex = 0;



void showSite(int index)
{
    Circle::draw_filled(&canvas, sites[index].x, sites[index].y, 1, EGA_BRIGHT_RED);

    for (int j = 0; j < sites[index].edges.size(); j++) {
        Line::draw(&canvas,
            sites[index].edges[j].x1, sites[index].edges[j].y1,
            sites[index].edges[j].x2, sites[index].edges[j].y2,
            EGA_LIGHT_CYAN
        );
        Circle::draw_filled(&canvas, sites[index].edges[j].x1, sites[index].edges[j].y1, 2, EGA_GREEN);
        Circle::draw(&canvas, sites[index].edges[j].x2, sites[index].edges[j].y2, 4, EGA_RED);
    }
}

int main()
{
    std::string seed = "1745774288";//std::to_string(time(NULL));

    srand(std::hash<std::string>()(seed));
    std::cout << "Seed: " << seed << std::endl;

    sites = new Site[siteCount];
    for (int i = 0; i < siteCount; i++) {
        sites[i].x = rand() % WIDTH;
        sites[i].y = rand() % HEIGHT;
    }
    Voronoi voronoi = Voronoi(WIDTH, HEIGHT, siteCount, sites);

    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) siteIndex = (siteIndex + 1) % siteCount;
    });

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        mouse.update(&openGL);

        // if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT)) {
        //     voronoi.setSite(
        //         mouse.xPos / PIXEL_SIZE,
        //         canvas.height - 1 - mouse.yPos / PIXEL_SIZE
        //     );
        // }





        // voronoi.create(canvas.height - 1 - mouse.yPos / PIXEL_SIZE);
        // voronoi.visualisation(&canvas, canvas.height - 1 -mouse.yPos / PIXEL_SIZE);

        voronoi.create();
        for (int i = 0; i < siteCount; i++) {
            Circle::draw(&canvas, sites[i].x, sites[i].y, 1, EGA_DARK_GREY);
            // showSite(i);
        }
        showSite(siteIndex);





        openGL.update(&canvas);
    }

    std::cout << "Seed: " << seed << std::endl;

    return EXIT_SUCCESS;
}
