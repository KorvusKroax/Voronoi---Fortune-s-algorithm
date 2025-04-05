#include "voronoi.h"
#include "circle.h"
#include "line.h"

#include <iostream>

Voronoi::Voronoi(unsigned int width, unsigned int height, int siteCount)
{
    this->width = width;
    this->height = height;
    this->siteCount = siteCount;

    this->sites = new int[this->siteCount * 2];
    for (int i = 0; i < this->siteCount; i++) {
        this->sites[i * 2] = rand() % this->width;
        this->sites[i * 2 + 1] = rand() % this->height;
    }
}

Voronoi::~Voronoi() { }

void Voronoi::draw(Canvas* canvas, int mx, int my)
{
    // for (int i = 0; i < this->siteCount; i++) {
    //     Circle::draw_filled(canvas, this->sites[i * 2], this->sites[i * 2 + 1], 1, EGA_RED);
    // }
    Circle::draw_filled(canvas, this->sites[0], this->sites[1], 1, EGA_RED);

    Line::draw(canvas, 0, my, canvas->width - 1, my, EGA_GREY);

    float yd = my;
    float xf = this->sites[0];
    float yf = this->sites[1];
    for (int x = 0; x < canvas->width; x++) {

        // int y = (1.0f / (2 * (yf - yd))) * ((x - xf) * (x - xf)) + ((yf + yd) / 2);
        int y = (((x - xf) * (x - xf)) / (2 * (yf - yd))) + ((yf + yd) / 2);

        canvas->setPixel(x, y, EGA_GREEN);
        //std::cout << "x: " << x << ", y: " << y << std::endl;
    }
}

void Voronoi::setSite(int x, int y)
{
    this->sites[0] = x;
    this->sites[1] = y;
}
