#include <cmath>
#include <random>
#include <iostream>
#include <vector>
#include <set>

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

Fortune* fortune;
int siteCount = 10;
int siteIndex = 0;



void createSites(std::string seed, std::vector<std::pair<double, double>>* points)
{
    srand(std::hash<std::string>()(seed));

    std::set<std::pair<int, int>> uniquePositions;
    while (uniquePositions.size() < siteCount) {
        uniquePositions.insert({rand() % WIDTH, rand() % HEIGHT});
    }

    for (std::pair<int, int> pos : uniquePositions) {
        (*points).push_back({pos.first, pos.second});
    }
}

// bool addSite(double x, double y)
// {
//     for (int j = 0; j < siteCount; j++) {
//         if (sites[j].x == x && sites[j].y == y) {
//             return false;
//         }
//     }

//     sites.push_back(Site(x, y));
//     return true;
// }

void showSite(Site* site, Color color)
{
    Circle::draw_filled(&canvas, site->x, site->y, 1, EGA_BRIGHT_RED);

    for (int j = 0; j < site->edges.size(); j++) {
        int x1 = site->edges[j]->x1;
        int y1 = site->edges[j]->y1;
        int x2 = site->edges[j]->x2;
        int y2 = site->edges[j]->y2;

        Line::draw(&canvas, x1, y1, x2, y2, color);
        Circle::draw_filled(&canvas, x1, y1, 2, EGA_GREEN);
        Circle::draw(&canvas, x2, y2, 4, EGA_RED);
    }
}

// void save(const char *filename)
// {
//     std::ofstream file_obj;
//     file_obj.open(filename, std::ios::out | std::ios::binary);

//     file_obj.write((char*)&siteCount, sizeof(int));
//     for (int i = 0; i < siteCount; i++) {
//         file_obj.write((char*)&sites[i].x, sizeof(double));
//         file_obj.write((char*)&sites[i].y, sizeof(double));
//     }

//     file_obj.close();
// }

// void load(const char *filename)
// {
//     std::ifstream file_obj;
//     file_obj.open(filename, std::ios::in | std::ios::binary);

//     file_obj.read((char*)&siteCount, sizeof(int));
//     sites.clear();
//     for (int i = 0; i < siteCount; i++) {
//         double x, y;
//         file_obj.read((char*)&x, sizeof(double));
//         file_obj.read((char*)&y, sizeof(double));
//         sites.push_back(Site(x, y));
//     }

//     file_obj.close();
// }



int main()
{
    std::string seed = "1745774288"; // 1745774288, 1748714277, 1748714344, 1748714424
    // std::string seed = std::to_string(time(NULL));

    glfwSetWindowPos(openGL.window, 100, 50);
    glfwSetKeyCallback(openGL.window, [](GLFWwindow* window, int key, int scancode, int action, int mod) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(openGL.window, true);
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) siteIndex = (siteIndex + 1) % siteCount;
        // if (key == GLFW_KEY_S && action == GLFW_PRESS) save("sites.map");
        // if (key == GLFW_KEY_L && action == GLFW_PRESS) load("sites.map");
    });




    std::vector<std::pair<double, double>> points;
    createSites(seed, &points);
    fortune = new Fortune(WIDTH, HEIGHT, &points);

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearCanvas();

        mouse.update(&openGL);

        // if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT)) {
        //     addSite(
        //         mouse.xPos / PIXEL_SIZE,
        //         canvas.height - 1 - mouse.yPos / PIXEL_SIZE
        //     );
        // }





        for (int i = 0; i < siteCount; i++) {
            Circle::draw(&canvas, fortune->sites[i].x, fortune->sites[i].y, 1, EGA_DARK_GREY);
            showSite(&fortune->sites[i], EGA_DARK_GREY);
        }
        showSite(&fortune->sites[siteIndex], EGA_WHITE);





        openGL.update(&canvas);
    }

    std::cout << "Seed: " << seed << std::endl;

    return EXIT_SUCCESS;
}
