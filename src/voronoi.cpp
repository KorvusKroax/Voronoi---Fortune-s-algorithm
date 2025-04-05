#include "voronoi.h"
#include "circle.h"

Voronoi::Voronoi(unsigned int width, unsigned int height, int siteCount)
{
    this->width = width;
    this->height = height;
    this->siteCount = siteCount;
}

Voronoi::~Voronoi() { }

void Voronoi::draw(Canvas* canvas)
{
    Circle::draw(canvas, canvas->width >> 1, canvas->height >> 1, 50, EGA_RED);
}
