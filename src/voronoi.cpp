#include <algorithm>
#include <iostream>

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

    beachLine.clear();

    for (int i = 0; i < this->siteCount; i++) {
        if (this->sites[i].y > sweepLine_y) break;

        // adding first parabola
        if (beachLine.empty()) {
            beachLine.push_back(std::make_unique<BeachLine::Parabola>(&this->sites[i]));
            continue;
        }

        // get the parabola's index below current site
        int index = 0;
        BeachLine::Parabola* curr_par = dynamic_cast<BeachLine::Parabola*>(beachLine[0].get());
        for (int j = 0; j < beachLine.size() - 1; j += 2) {
            BeachLine::Parabola* next_par = dynamic_cast<BeachLine::Parabola*>(beachLine[j + 2].get());

            float intersect_x = getIntersect_x(
                curr_par->site->x, curr_par->site->y,
                next_par->site->x, next_par->site->y,
                sweepLine_y
            );

            if (this->sites[i].x < intersect_x) {
                index = j;
                break;
            }
            curr_par = next_par;
        }

        // duplicate the parabola found
        BeachLine::Parabola* par = dynamic_cast<BeachLine::Parabola*>(beachLine[index].get());
        beachLine.insert(beachLine.begin() + index, std::make_unique<BeachLine::Parabola>(par->site));

        // insert two edges beside the new parabola
        float dx = par->site->x - this->sites[i].x;
        float dy = par->site->y - this->sites[i].y;
        float edgeStart_y = (float(dx * dx) / (2 * dy)) + ((par->site->y + this->sites[i].y) >> 1);

        // left edge
        beachLine.insert(beachLine.begin() + index + 1, std::make_unique<BeachLine::Edge>(this->sites[i].x, edgeStart_y, dx, -dy));
        // new parabola
        beachLine.insert(beachLine.begin() + index + 2, std::make_unique<BeachLine::Parabola>(&this->sites[i]));
        // rigth edge
        beachLine.insert(beachLine.begin() + index + 3, std::make_unique<BeachLine::Edge>(this->sites[i].x, edgeStart_y, -dx, dy));
    }




    if (beachLine.empty()) return;



    // console log

    // for (int i = 0; i < beachLine.size(); i += 1) {
    //     auto* par = dynamic_cast<Beachline::Base*>(beachLine[i].get());
    //     std::cout << i << ": " << (par->getType() == Beachline::Type::EDGE ? "edge" : "parabola") << std::endl;
    // }
    // std::cout << std::endl;



    // visualisation

    // draw all full parabolas
    for (int i = 0; i < beachLine.size(); i += 2) {
        BeachLine::Parabola* par = dynamic_cast<BeachLine::Parabola*>(beachLine[i].get());
        drawParabola(canvas, par->site->x, par->site->y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
    }

    // draw beachline
    BeachLine::Parabola* curr_par = dynamic_cast<BeachLine::Parabola*>(beachLine[0].get());
    float min_x = 0;
    for (int i = 0; i < beachLine.size() - 1; i += 2) {
        BeachLine::Parabola* next_par = dynamic_cast<BeachLine::Parabola*>(beachLine[i + 2].get());
        float max_x = (i < beachLine.size() - 1) ?
            getIntersect_x(curr_par->site->x, curr_par->site->y, next_par->site->x, next_par->site->y, sweepLine_y) :
            canvas->width - 1;

        // draw parabola
        drawParabola(canvas, curr_par->site->x, curr_par->site->y, sweepLine_y, min_x, max_x, EGA_GREEN);

        // draw halfedges
        BeachLine::Edge* edge = dynamic_cast<BeachLine::Edge*>(beachLine[i + 1].get());
        float dx = max_x - edge->x;
        Line::draw(canvas,
            edge->x,
            edge->y,
            edge->x + dx,
            edge->y + dx * edge->slope,
            EGA_CYAN
        );

        // draw edge's start
        canvas->setPixel(edge->x, edge->y, EGA_WHITE);

        curr_par = next_par;
        min_x = max_x;
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
