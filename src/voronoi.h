#pragma once

#include "canvas.h"
#include "site.h"

class Voronoi {
public:
    unsigned int width;
    unsigned int height;
    unsigned int siteCount;
    Site* sites;

    Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites);
    ~Voronoi();

    void show(Canvas* canvas, int sweepLine_y);
    void drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y);
};
