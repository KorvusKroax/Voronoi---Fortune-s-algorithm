#pragma once

#include <vector>
#include <memory>

#include "canvas.h"

#include "site.h"
#include "event.h"
#include "beachline.h"
#include "edge.h"

class Voronoi {
    public:
        unsigned int width;
        unsigned int height;
        unsigned int siteCount;
        Site* sites;

        std::vector<Event> events;
        std::vector<BeachLine> beachLine;
        int beachLineCounter;

        Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites);

        void create(int sweepLine_y);
        double getIntersect_x(double parLeft_x, double parLeft_y, double parRight_x, double parRight_y, double sweepLine_y);
        void checkCircleEvent(int parabolaIndex);

        void visualisation(Canvas* canvas, int sweepLine_y);
        void drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y, int minX, int maxX, Color color);
};
