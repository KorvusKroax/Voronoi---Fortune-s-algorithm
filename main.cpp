#include <cmath>
#include <random>
#include <iostream>

#include "canvas.h"
#include "open_gl.h"
#include "mouse.h"
#include "line.h"
#include "circle.h"

#include "src/fortune.h"
#include "src/site.h"



const int WIDTH = 256;
const int HEIGHT = 256;
const float PIXEL_SIZE = 3;

Canvas canvas = Canvas(WIDTH, HEIGHT);
OpenGL openGL = OpenGL(&canvas, PIXEL_SIZE, WINDOWED_RESIZABLE);
Mouse mouse = Mouse(&openGL, 1, MOUSE_CURSOR_ENABLED);

Fortune* voronoi = new Fortune();
Site* sites;
int siteCount = 5;
int siteIndex = 0;



void showSite(int index)
{
    Circle::draw_filled(&canvas, sites[index].x, sites[index].y, 1, EGA_BRIGHT_RED);

    for (int j = 0; j < sites[index].edges.size(); j++) {
        Line::draw(&canvas,
            sites[index].edges[j]->x, sites[index].edges[j]->y,
            sites[index].edges[j]->x + sites[index].edges[j]->dir_x, sites[index].edges[j]->y + sites[index].edges[j]->dir_y,
            EGA_LIGHT_CYAN
        );
        Circle::draw_filled(&canvas, sites[index].edges[j]->x, sites[index].edges[j]->y, 2, EGA_GREEN);
        Circle::draw(&canvas, sites[index].edges[j]->x + sites[index].edges[j]->dir_x, sites[index].edges[j]->y + sites[index].edges[j]->dir_y, 4, EGA_RED);
    }
}



int main()
{
    // 1745774288, 1748714277, 1748714344, 1748714424
    std::string seed = "1745774288";
    // std::string seed = std::to_string(time(NULL));

    srand(std::hash<std::string>()(seed));
    std::cout << "Seed: " << seed << std::endl;

    sites = new Site[siteCount];
    for (int i = 0; i < siteCount; i++) {
        sites[i].x = rand() % WIDTH;
        sites[i].y = rand() % HEIGHT;
    }

    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) siteIndex = (siteIndex + 1) % siteCount;
    });

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        mouse.update(&openGL);





        voronoi->create(WIDTH, HEIGHT, siteCount, sites);
        for (int i = 0; i < siteCount; i++) {
            Circle::draw(&canvas, sites[i].x, sites[i].y, 1, EGA_DARK_GREY);
            showSite(i);
        }
        showSite(siteIndex);

        for (int j = 0; j < voronoi->finishedEdges.size(); j++) {
            Line::draw(&canvas,
                voronoi->finishedEdges[j]->x, voronoi->finishedEdges[j]->y,
                voronoi->finishedEdges[j]->x + voronoi->finishedEdges[j]->dir_x, voronoi->finishedEdges[j]->y + voronoi->finishedEdges[j]->dir_y,
                EGA_LIGHT_BLUE
            );
            Circle::draw_filled(&canvas, voronoi->finishedEdges[j]->x, voronoi->finishedEdges[j]->y, 2, EGA_GREEN);
            Circle::draw(&canvas, voronoi->finishedEdges[j]->x + voronoi->finishedEdges[j]->dir_x, voronoi->finishedEdges[j]->y + voronoi->finishedEdges[j]->dir_y, 4, EGA_RED);
        }





        openGL.update(&canvas);
    }

    std::cout << "Seed: " << seed << std::endl;

    return EXIT_SUCCESS;
}
