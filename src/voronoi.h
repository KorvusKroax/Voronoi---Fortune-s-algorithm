#pragma once

#include <vector>
#include <memory>

#include "canvas.h"

#include "site.h"
#include "beachline.h"

class Voronoi {
    public:
        unsigned int width;
        unsigned int height;
        unsigned int siteCount;
        Site* sites;

        std::vector<std::unique_ptr<BeachLine::Base>> beachLine;

        Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites);
        ~Voronoi();

        void show(Canvas* canvas, int sweepLine_y);
        void drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y, int minX, int maxX, Color color);

        double getIntersect_x(double parLeft_x, double parLeft_y, double parRight_x, double parRight_y, double sweepLine_y);
};
