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
int siteCount = 10;
int siteIndex = 0;



void createSites(std::string seed)
{
    srand(std::hash<std::string>()(seed));

    sites = new Site[siteCount];
    for (int i = 0; i < siteCount; i++) {
        sites[i].x = rand() % WIDTH;
        sites[i].y = rand() % HEIGHT;
    }
}

void showSite(int index)
{
    Circle::draw_filled(&canvas, sites[index].x, sites[index].y, 1, EGA_BRIGHT_RED);

    for (int j = 0; j < sites[index].edges.size(); j++) {

        if (HalfEdge* he = dynamic_cast<HalfEdge*>(sites[index].edges[j])) {
            int x1, y1;
            if (he->otherHalf == nullptr) {
                x1 = he->x;
                y1 = he->y;
            } else {
                x1 = he->otherHalf->x + he->otherHalf->dir_x;
                y1 = he->otherHalf->y + he->otherHalf->dir_y;
            }
            int x2 = he->x + he->dir_x;
            int y2 = he->y + he->dir_y;

            Line::draw(&canvas, x1, y1, x2, y2, EGA_CYAN);
            Circle::draw_filled(&canvas, x1, y1, 2, EGA_GREEN);
            Circle::draw(&canvas, x2, y2, 4, EGA_RED);

        } else {

            int x1 = sites[index].edges[j]->x;
            int y1 = sites[index].edges[j]->y;
            int x2 = sites[index].edges[j]->x2;
            int y2 = sites[index].edges[j]->y2;

            Line::draw(&canvas, x1, y1, x2, y2, EGA_WHITE);
            Circle::draw_filled(&canvas, x1, y1, 2, EGA_LIGHT_GREEN);
            Circle::draw(&canvas, x2, y2, 4, EGA_BRIGHT_RED);

        }

    }
}



int main()
{
    std::string seed = "1745774288"; // 1745774288, 1748714277, 1748714344, 1748714424
    // std::string seed = std::to_string(time(NULL));

    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) siteIndex = (siteIndex + 1) % siteCount;
    });

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        mouse.update(&openGL);



        createSites(seed);

        voronoi->create(WIDTH, HEIGHT, siteCount, sites);
        for (int i = 0; i < siteCount; i++) {
            Circle::draw(&canvas, sites[i].x, sites[i].y, 1, EGA_DARK_GREY);
            showSite(i);
        }

        // showSite(siteIndex);
        // std::cout << "edgeCount: " << sites[siteIndex].edges.size() << std::endl;





        openGL.update(&canvas);
    }

    std::cout << "Seed: " << seed << std::endl;

    return EXIT_SUCCESS;
}
