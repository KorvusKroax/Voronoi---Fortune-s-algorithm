#include <algorithm>
#include <iostream>
#include <vector>

#include "circle.h"
#include "line.h"

#include "voronoi.h"

Voronoi::Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites)
{
    this->width = width;
    this->height = height;
    this->siteCount = siteCount;
    this->sites = sites;

    std::sort(this->sites, this->sites + this->siteCount,
        [](Site const & a, Site const & b) -> bool {
            return a.y < b.y;
        }
    );
}

Voronoi::~Voronoi() { }

void Voronoi::show(Canvas* canvas, int sweepLine_y)
{
    // sites
    for (int i = 0; i < this->siteCount; i++) {
        Circle::draw_filled(canvas, this->sites[i].x, this->sites[i].y, 1, EGA_RED);
    }

    // sweep line
    Line::draw(canvas, 0, sweepLine_y, canvas->width - 1, sweepLine_y, EGA_GREY);



    // std::vector<Event> events;
    // for (int i = 0; i < this->siteCount; i++) events.push_back(SiteEvent(this->sites[i].y, BeachlineParabola(this->sites[i])))

    std::vector<BeachlineParabola> beachLine;
    std::vector<CircleEvent> circleEvents;
    for (int i = 0; i < this->siteCount; i++) {
        if (this->sites[i].y > sweepLine_y) break;

        // adding first parabola
        if (beachLine.empty()) {
            beachLine.push_back(BeachlineParabola(this->sites[i]));
            continue;
        }

        // get the parabola's index below current site
        int index = 0;
        for (int j = 0; j < beachLine.size() - 1; j++) {
            if (this->sites[i].x < getIntersect_x(beachLine[j].site.x, beachLine[j].site.y, beachLine[j + 1].site.x, beachLine[j + 1].site.y, sweepLine_y)) {
                index = j;
                break;
            }
        }

        // insert the new parabola into the beachLine
        float dx = beachLine[index].site.x - this->sites[i].x;
        float dy = beachLine[index].site.y - this->sites[i].y;
        float edgeStart_y = (float(dx * dx) / (2 * dy)) + ((beachLine[index].site.y + this->sites[i].y) >> 1);

        beachLine.insert(beachLine.begin() + index, beachLine[index]);
        index++;
        beachLine.insert(beachLine.begin() + index,
            BeachlineParabola(this->sites[i],
                new HalfEdge(this->sites[i].x, edgeStart_y, -dx / dy),
                new HalfEdge(this->sites[i].x, edgeStart_y, dx / -dy)
            )
        );

        // check circumcircle to left
        if (index - 2 >= 0) {
            float cx, cy, cr;
            if (getCircumcircle(
                beachLine[index - 2].site.x, beachLine[index - 2].site.y,
                beachLine[index - 1].site.x, beachLine[index - 1].site.y,
                beachLine[index].site.x    , beachLine[index].site.y,
                &cx, &cy, &cr
            )) {
                circleEvents.push_back(CircleEvent(cx, cy, cr));
            }
        }

        // check circumcircle to right
        if (index + 2 < beachLine.size()) {
            float cx, cy, cr;
            if (getCircumcircle(
                beachLine[index].site.x    , beachLine[index].site.y,
                beachLine[index + 1].site.x, beachLine[index + 1].site.y,
                beachLine[index + 2].site.x, beachLine[index + 2].site.y,
                &cx, &cy, &cr
            )) {
                circleEvents.push_back(CircleEvent(cx, cy, cr));
            }
        }
    }



    // visualisation

    if (beachLine.empty()) return;

    // // info to console
    // for (int i = 0; i < beachLine.size(); i++) {
    //     std::cout << "beachLine[" << i << "] = (x" << beachLine[i].site.x << ", y" << beachLine[i].site.y << ")" << std::endl;
    // }
    // std::cout << std::endl;

    // draw all full parabolas
    for (int i = 0; i < beachLine.size(); i++) {
        drawParabola(canvas, beachLine[i].site.x, beachLine[i].site.y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
    }

    float max_x, min_x = 0;
    for (int i = 0; i < beachLine.size(); i++) {
        max_x = (i < beachLine.size() - 1) ?
            getIntersect_x(beachLine[i].site.x, beachLine[i].site.y, beachLine[i + 1].site.x, beachLine[i + 1].site.y, sweepLine_y) :
            canvas->width - 1;

        // draw beachline
        drawParabola(canvas, beachLine[i].site.x, beachLine[i].site.y, sweepLine_y, min_x, max_x, EGA_GREEN);

        // draw parabola halfedges
        if (beachLine[i].halfEdge_left != nullptr) {
            float dx = min_x - beachLine[i].halfEdge_left->x;
            Line::draw(canvas,
                beachLine[i].halfEdge_left->x,
                beachLine[i].halfEdge_left->y,
                beachLine[i].halfEdge_left->x + dx,
                beachLine[i].halfEdge_left->y + dx * beachLine[i].halfEdge_left->slope,
                EGA_CYAN
            );
            canvas->setPixel(beachLine[i].halfEdge_left->x, beachLine[i].halfEdge_left->y, EGA_WHITE);
        }
        if (beachLine[i].halfEdge_right != nullptr) {
            float dx = max_x - beachLine[i].halfEdge_right->x;
            Line::draw(canvas,
                beachLine[i].halfEdge_right->x,
                beachLine[i].halfEdge_right->y,
                beachLine[i].halfEdge_right->x + dx,
                beachLine[i].halfEdge_right->y + dx * beachLine[i].halfEdge_right->slope,
                EGA_ORANGE
            );
            canvas->setPixel(beachLine[i].halfEdge_right->x, beachLine[i].halfEdge_right->y, EGA_WHITE);
        }

        min_x = max_x;
    }

    if (circleEvents.empty()) return;

    // draw circumcircles
    for (int i = 0; i < circleEvents.size(); i++) {
        Circle::draw(canvas, circleEvents[i].x, circleEvents[i].y, circleEvents[i].r, EGA_MAGENTA);
    }
}

void Voronoi::drawParabola(Canvas* canvas, int focus_x, int focus_y, int sweepLine_y, int min_x, int max_x, Color color)
{
    if (focus_y == sweepLine_y) return;
    for (int x = min_x; x < max_x; x++) {
        // int y = (1.0f / (2 * (focus_y - sweepLine_y))) * ((x - focus_x) * (x - focus_x)) + ((focus_y + sweepLine_y) / 2);
        int y = (float((x - focus_x) * (x - focus_x)) / (2 * (focus_y - sweepLine_y))) + ((focus_y + sweepLine_y) >> 1);
        canvas->setPixel(x, y, color);
    }
}

double Voronoi::getIntersect_x(double site_x, double site_y, double nextSite_x, double nextSite_y, double sweepLine_y)
{
    if (site_y == sweepLine_y) return site_x;
    if (nextSite_y == sweepLine_y) return nextSite_x;

    double dp = 2.0 * (site_y - sweepLine_y);
    double a1 = 1.0 / dp;
    double b1 = -2.0 * site_x / dp;
    double c1 = sweepLine_y + dp / 4.0 + (site_x * site_x) / dp;

           dp = 2.0 * (nextSite_y - sweepLine_y);
    double a2 = 1.0 / dp;
    double b2 = -2.0 * nextSite_x / dp;
    double c2 = sweepLine_y + dp / 4.0 + (nextSite_x * nextSite_x) / dp;

    double a = a1 - a2;
    double b = b1 - b2;
    double c = c1 - c2;

    double sqrtDiscriminant = std::sqrt(b * b - 4.0 * a * c);
    double x1 = (-b + sqrtDiscriminant) / (2 * a);
    double x2 = (-b - sqrtDiscriminant) / (2 * a);

    return site_y > nextSite_y ? std::max(x1, x2) : std::min(x1, x2);
}

bool Voronoi::getCircumcircle(float x1, float y1, float x2, float y2, float x3, float y3, float* x, float* y, float* r)
{
    if ((x1 == x2 && y1 == y2) || (x2 == x3 && y2 == y3) || (x3 == x1 && y3 == y1)) return false;

    float perp_12_x1 = x1;
    float perp_12_y1 = y1;
    float perp_12_x2 = x2;
    float perp_12_y2 = y2;
    setPerpendicular(&perp_12_x1, &perp_12_y1, &perp_12_x2, &perp_12_y2);

    float perp_23_x1 = x2;
    float perp_23_y1 = y2;
    float perp_23_x2 = x3;
    float perp_23_y2 = y3;
    setPerpendicular(&perp_23_x1, &perp_23_y1, &perp_23_x2, &perp_23_y2);

    if (!lineIntersection(perp_12_x1, perp_12_y1, perp_12_x2, perp_12_y2, perp_23_x1, perp_23_y1, perp_23_x2, perp_23_y2, x, y)) {
        return false;
    }

    float dx = *x - x1;
    float dy = *y - y1;
    *r = sqrt(dx * dx + dy * dy);

    return true;
}

void Voronoi::setPerpendicular(float* x1, float* y1, float* x2, float* y2)
{
    float dx = *x2 - *x1;
    float dy = *y2 - *y1;

    float center_x = (*x1 + *x2) / 2;
    float center_y = (*y1 + *y2) / 2;

    *x1 = center_x + dy;
    *y1 = center_y - dx;
    *x2 = center_x - dy;
    *y2 = center_y + dx;
}

bool Voronoi::lineIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* ix, float* iy)
{
    float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (den == 0) return false;

    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
    if (0 > t || t > 1) return false;

    float u = ((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / -den;
    if (0 > u || u > 1) return false;

    *ix = x1 + (x2 - x1) * t;
    *iy = y1 + (y2 - y1) * t;
    return true;
}




/*

    A = x1 * (y2 − y3) − y1 * (x2 − x3) + x2 * y3 − x3 * y2
    B = (x1*x1 + y1*y1) * (y3 − y2) + (x2*x2 + y2*y2) * (y1 − y3) + (x3*x3 + y3*y3) * (y2 − y1)
    C = (x1*x1 + y1*y1) * (x2 − x3) + (x2*x2 + y2*y2) * (x3 − x1) + (x3*x3 + y3*y3) * (x1 − x2)
    D = (x1*x1 + y1*y1) * (x3*y2 − x2*y3) + (x2*x2 + y2*y2) * (x1*y3 − x3*y1) + (x3*x3 + y3*y3) * (x2*y1 − x1*y2)

    If A=0 then the points are colinear elasewhere using A,B,C you can find center and radius of circle

    xc = -B / 2 * A
    yc = -C / 2 * A
    r  = sqrt((B*B + C*C - 4 * A * D) / (4 * (A * A)))

*/

// bool Voronoi::getCircumcircle_(float x1, float y1, float x2, float y2, float x3, float y3, float* x, float* y, float* r)
// {
//     float A = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
//     float B = (x1*x1 + y1*y1) * (y3 - y2) + (x2*x2 + y2*y2) * (y1 - y3) + (x3*x3 + y3*y3) * (y2 - y1);
//     float C = (x1*x1 + y1*y1) * (x2 - x3) + (x2*x2 + y2*y2) * (x3 - x1) + (x3*x3 + y3*y3) * (x1 - x2);
//     float D = (x1*x1 + y1*y1) * (x3*y2 - x2*y3) + (x2*x2 + y2*y2) * (x1*y3 - x3*y1) + (x3*x3 + y3*y3) * (x2*y1 - x1*y2);

//     // If A=0 then the points are colinear elasewhere using A,B,C you can find center and radius of circle
//     if (A == 0) return false;

//     *x = -B / 2 * A;
//     *y = -C / 2 * A;
//     *r  = sqrt((B*B + C*C - 4 * A * D) / (4 * (A * A)));

//     return true;
// }
