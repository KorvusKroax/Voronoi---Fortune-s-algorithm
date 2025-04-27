#pragma once

#include "canvas.h"

#include "site.h"
#include "event.h"
#include "beachline_parabola.h"
#include "halfedge.h"

class Voronoi {
public:
    unsigned int width;
    unsigned int height;
    unsigned int siteCount;
    Site* sites;

    Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites);
    ~Voronoi();

    void show(Canvas* canvas, int sweepLine_y);
    void drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y, int minX, int maxX, Color color);

    double getIntersect_x(double parLeft_x, double parLeft_y, double parRight_x, double parRight_y, double sweepLine_y);
    bool getCircumcircle(float x1, float y1, float x2, float y2, float x3, float y3, float* x, float* y, float* r);
    void setPerpendicular(float* x1, float* y1, float* x2, float* y2);
    bool lineIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* ix, float* iy);

    // bool getCircumcircle_(float x1, float y1, float x2, float y2, float x3, float y3, float* x, float* y, float* r);
};
