#pragma once

#include <vector>
#include <memory>

#include "canvas.h"

#include "site.h"
#include "event.h"
#include "beachline.h"
#include "half_edge.h"
#include "edge.h"

class Voronoi {
    public:
        unsigned int width;
        unsigned int height;
        unsigned int siteCount;
        Site* sites;

        std::vector<Event> events;
        std::vector<Beachline> beachline;
        int beachline_id;

        Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites);

        void init();
        void create();
        void create(int sweepLine_y);

        void siteEvent(Event* curr_event);
        void circleEvent(Event* curr_event);

        void checkAndAddCircleEvent(int parabola_index);
        void checkAndAddCircleEvent_circle(int parabola_index);
        void checkAndRemoveCircleEvent(int parabola_index);
        void finishingHalfEdges();

        int getBeachlineIndexBelow(Site* curr_event_site);
        float getParabolaIntersect_x(float focus_x, float focus_y, float nextFocus_x, float nextFocus_y, float sweepLine_y);



        void visualisation(Canvas* canvas, float sweepLine_y);
        void drawParabola(Canvas* canvas, float xf, float yf, float sweepLine_y, float minX, float maxX, Color color);
};
