#pragma once

#include "canvas.h"

class Voronoi {
    public:
        unsigned int width;
        unsigned int height;
        unsigned int siteCount;
        int* sites;

        Voronoi(unsigned int width, unsigned int height, int siteCount);
        ~Voronoi();

        void draw(Canvas* canvas, int x, int y);
        void setSite(int x, int y);
};
