#pragma once

#include <vector>
#include <memory>

#include "canvas.h"

#include "site.h"
#include "event.h"
#include "beachline.h"
#include "half_edge.h"

class Voronoi {
    public:
        unsigned int width;
        unsigned int height;
        unsigned int siteCount;
        Site* sites;

        std::vector<Event> events;
        std::vector<Beachline> beachline;
        int beachline_id;

        std::vector<HalfEdge> finishedHalfEdges;

        Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites);

        void create();
        void create(int sweepLine_y);
        void init();

        void siteEvent(Event* curr_event);
        int getBeachlineIndexBelow(Site* curr_event_site);
        void circleEvent(Event* curr_event);

        double getParabolaIntersect_x(double parLeft_x, double parLeft_y, double parRight_x, double parRight_y, double sweepLine_y);
        void checkAndAddCircleEvent(int parabola_index);
        void checkAndRemoveCircleEvent(int parabola_index);
        void finishingHalfEdges();

        void visualisation(Canvas* canvas);
        void visualisation(Canvas* canvas, int sweepLine_y);
        void drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y, int minX, int maxX, Color color);
};
