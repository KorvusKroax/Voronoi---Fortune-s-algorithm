#pragma once

#include "canvas.h"

class Voronoi {
    public:
        unsigned int width;
        unsigned int height;
        unsigned int siteCount;

        Voronoi(unsigned int width, unsigned int height, int siteCount);
        ~Voronoi();

        void draw(Canvas* canvas);
};
