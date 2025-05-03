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
}

Voronoi::~Voronoi() { }

void Voronoi::create(int sweepLine_y)
{
    this->events.clear();
    this->beachLine.clear();
    this->edges.clear();

    // set events (add site events)
    for (int i = 0; i < this->siteCount; i++) {
        this->events.push_back(Event(SITE, this->sites[i].y, i));
    }
    // sort events by Y
    std::sort(this->events.begin(), this->events.end(), [](const Event& lhs, const Event& rhs) -> bool { return lhs.y < rhs.y; });

    int count = 0;
    while (!this->events.empty() && count < 1000) {
        Event* curr_event = &this->events.front();
        if (curr_event->y > sweepLine_y) break;

        if (curr_event->type == SITE) {

            if (this->beachLine.empty()) {
                // adding first parabola (it will be placed out from loop as begining value of events)
                this->beachLine.push_back(BeachLine(PARABOLA, curr_event->index));
            } else {
                int index = 0;
                if (this->beachLine.size() > 1) {
                    // get the parabola's index below current site
                    for (int i = 0; i < this->beachLine.size() - 1; i += 2) {
                        float intersect_x = getIntersect_x(
                            this->sites[this->beachLine[i].index].x, this->sites[this->beachLine[i].index].y,
                            this->sites[this->beachLine[i + 2].index].x, this->sites[this->beachLine[i + 2].index].y,
                            sweepLine_y
                        );
                        if (this->sites[curr_event->index].x < intersect_x) {
                            index = i;
                            break;
                        }
                    }
                }

                // duplicate the parabola found
                int parabola_below_index = this->beachLine[index].index;
                this->beachLine.insert(this->beachLine.begin() + index, BeachLine(PARABOLA, parabola_below_index));

                // insert new parabola with two edges beside it
                float dx = this->sites[parabola_below_index].x - this->sites[curr_event->index].x;
                float dy = this->sites[parabola_below_index].y - this->sites[curr_event->index].y;
                float edgeStart_y = (float(dx * dx) / (2 * dy)) + (float(this->sites[parabola_below_index].y + this->sites[curr_event->index].y) / 2);

                this->beachLine.insert(this->beachLine.begin() + index + 1, BeachLine(EDGE, this->edges.size()));
                this->edges.push_back(Edge(this->sites[curr_event->index].x, edgeStart_y, -dy, dx));

                this->beachLine.insert(this->beachLine.begin() + index + 2, BeachLine(PARABOLA, curr_event->index));

                this->beachLine.insert(this->beachLine.begin() + index + 3, BeachLine(EDGE, this->edges.size()));
                this->edges.push_back(Edge(this->sites[curr_event->index].x, edgeStart_y, dy, -dx));

                // check and add circle events if needed
                checkCircleEvent(index);
                checkCircleEvent(index + 4);
            }
        } else {
            // circle event
            std::cout << "circle event" << std::endl;
        }

        // remove event
        this->events.erase(this->events.begin());
        count++;
    }

    if (count == 1000) std::cout << "INFINITE" << std::endl;
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

void Voronoi::checkCircleEvent(int parabola_index)
{
    if (parabola_index < 2 || parabola_index > this->beachLine.size() - 2) return;

    Edge* edge_left = &this->edges[this->beachLine[parabola_index - 1].index];
    Site* parabola = &this->sites[this->beachLine[parabola_index].index];
    Edge* edge_right = &this->edges[this->beachLine[parabola_index + 1].index];

    float cx, cy;
    if (Line::intersection_ray(
            edge_left->x, edge_left->y, edge_left->dx, edge_left->dy,
            edge_right->x, edge_right->y, edge_right->dx, edge_right->dy,
            &cx, &cy)
    ) {
        float dx = parabola->x - cx;
        float dy = parabola->y - cy;
        float r = sqrt(dx * dx + dy * dy);

        // add new circle event
        int index = 0; // can it be 0 ???
        while (index < this->events.size()) {
            if (cy + r < this->events[index].y) break;
            index++;
        }
        this->events.insert(this->events.begin() + index, Event(CIRCLE, cy + r, parabola_index));
    }
}



void Voronoi::visualisation(Canvas* canvas, int sweepLine_y)
{
    // events
    for (int i = 0; i < this->events.size(); i++) {
        Line::draw(canvas, 0, this->events[i].y, canvas->width - 1, this->events[i].y,
            this->events[i].type == SITE ? EGA_RED : EGA_MAGENTA);
    }

    // sweep line
    Line::draw(canvas, 0, sweepLine_y, canvas->width - 1, sweepLine_y, EGA_GREY);

    // sites
    for (int i = 0; i < this->siteCount; i++) {
        Circle::draw_filled(canvas, this->sites[i].x, this->sites[i].y, 1, EGA_BRIGHT_RED);
    }

    if (this->beachLine.empty()) return;

    // draw all full parabolas
    for (int i = 0; i < this->beachLine.size(); i += 2) {
        drawParabola(canvas, this->sites[this->beachLine[i].index].x, this->sites[this->beachLine[i].index].y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
    }

    // draw beachline
    float min_x = 0;
    for (int i = 0; i < this->beachLine.size() - 1; i += 2) {
        float max_x = (i < this->beachLine.size() - 1) ?
            getIntersect_x(
                this->sites[this->beachLine[i].index].x, this->sites[this->beachLine[i].index].y,
                this->sites[this->beachLine[i + 2].index].x, this->sites[this->beachLine[i + 2].index].y,
                sweepLine_y
            ) :
            canvas->width - 1;

        // draw parabola
        drawParabola(canvas,
            this->sites[this->beachLine[i].index].x, this->sites[this->beachLine[i].index].y,
            sweepLine_y, min_x, max_x, EGA_GREEN);

        // draw halfedges
        Edge* edge = &this->edges[this->beachLine[i + 1].index];
        float slope = edge->dy / edge->dx;
        float dx = max_x - edge->x;
        Line::draw(canvas, edge->x, edge->y, edge->x + dx, edge->y + dx * slope, EGA_CYAN);
        canvas->setPixel(edge->x, edge->y, EGA_WHITE);

        min_x = max_x;
    }

    // draw circle events
    for (int i = 0; i < this->events.size(); i++) {
        if (this->events[i].type != CIRCLE) continue;

        Edge* edge_left = &this->edges[this->beachLine[this->events[i].index - 1].index];
        Site* parabola = &this->sites[this->beachLine[this->events[i].index].index];
        Edge* edge_right = &this->edges[this->beachLine[this->events[i].index + 1].index];

        // // helpers
        // Line::draw(canvas, edge_left->x, edge_left->y, edge_left->x - edge_left->dx, edge_left->y + edge_left->dy, EGA_LIGHT_BLUE);
        // Circle::draw(canvas, edge_left->x, edge_left->y, 2, EGA_LIGHT_BLUE);
        // Line::draw(canvas, edge_right->x, edge_right->y, edge_right->x - edge_right->dx, edge_right->y - edge_right->dy, EGA_ORANGE);
        // Circle::draw(canvas, edge_right->x, edge_right->y, 2, EGA_ORANGE);

        float cx, cy;
        if (Line::intersection_ray(
                edge_left->x, edge_left->y, edge_left->dx, edge_left->dy,
                edge_right->x, edge_right->y, edge_right->dx, edge_right->dy,
                &cx, &cy)
        ) {
            float dx = parabola->x - cx;
            float dy = parabola->y - cy;
            float r = sqrt(dx * dx + dy * dy);
            Circle::draw(canvas, cx, cy, r, EGA_BRIGHT_MAGENTA);
            Circle::draw(canvas, cx, cy, 1, EGA_BRIGHT_MAGENTA);
        }
    }
}

void Voronoi::drawParabola(Canvas* canvas, int focus_x, int focus_y, int sweepLine_y, int min_x, int max_x, Color color)
{
    if (focus_y == sweepLine_y) return;
    for (int x = min_x; x < max_x; x++) {
        // int y = (1.0f / (2 * (focus_y - sweepLine_y))) * ((x - focus_x) * (x - focus_x)) + ((focus_y + sweepLine_y) / 2);
        int y = (float((x - focus_x) * (x - focus_x)) / (2 * (focus_y - sweepLine_y))) + ((focus_y + sweepLine_y) / 2);
        canvas->setPixel(x, y, color);
    }
}
