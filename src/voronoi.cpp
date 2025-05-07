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

void Voronoi::create(int sweepLine_y)
{
    this->events.clear();
    this->beachLine.clear(); beachLineCounter = 0;

    this->finishedHalfEdges.clear();

    // set events (add site events)
    for (int i = 0; i < this->siteCount; i++) {
        this->events.push_back(Event(SITE, this->sites[i].y, &this->sites[i]));
    }
    // sort events by Y
    std::sort(this->events.begin(), this->events.end(), [](const Event& lhs, const Event& rhs) -> bool { return lhs.y < rhs.y; });

    int count = 0; // temporary
    while (!this->events.empty() && count < 1000) {
        Event* curr_event = &this->events.front();
        if (curr_event->y > sweepLine_y) break;

        if (curr_event->type == SITE) {

            if (this->beachLine.empty()) {
                // adding first parabola (it will be placed out from loop as begining value of events)
                this->beachLine.push_back(BeachLine(beachLineCounter++, PARABOLA, curr_event->ptr));
            } else {

                Site* curr_event_site = static_cast<Site*>(curr_event->ptr);
                int index = 0;
                if (this->beachLine.size() > 1) {
                    // get the parabola's index below current site
                    for (int i = 0; i < this->beachLine.size() - 1; i += 2) {
                        Site* curr_par = static_cast<Site*>(beachLine[i].ptr);
                        Site* next_par = static_cast<Site*>(beachLine[i + 2].ptr);
                        float intersect_x = getIntersect_x(
                            curr_par->x, curr_par->y,
                            next_par->x, next_par->y,
                            curr_event->y
                        );
                        if (curr_event_site->x < intersect_x) {
                            index = i;
                            break;
                        }
                    }
                }

                Site* parabola = static_cast<Site*>(this->beachLine[index].ptr);

                // check if parabola has a circle event then remove that circle event
                // ...

                // duplicate the parabola found
                this->beachLine.insert(this->beachLine.begin() + index, BeachLine(beachLineCounter++, PARABOLA, parabola));

                // insert new parabola with two edges beside it
                float dx = parabola->x - curr_event_site->x;
                float dy = parabola->y - curr_event_site->y;
                float edgeStart_y = (float(dx * dx) / (2 * dy)) + (float(parabola->y + curr_event_site->y) / 2);

                Edge* edge_left = new Edge(curr_event_site->x, edgeStart_y, -dy, dx);
                this->beachLine.insert(this->beachLine.begin() + index + 1, BeachLine(beachLineCounter++, EDGE, edge_left));

                this->beachLine.insert(this->beachLine.begin() + index + 2, BeachLine(beachLineCounter++, PARABOLA, curr_event->ptr));

                Edge* edge_right = new Edge(curr_event_site->x, edgeStart_y, dy, -dx);
                this->beachLine.insert(this->beachLine.begin() + index + 3, BeachLine(beachLineCounter++, EDGE, edge_right));

                // check and add circle events if needed
                checkCircleEvent(index);
                checkCircleEvent(index + 4);
            }

        } else if (curr_event->type == CIRCLE) {

            BeachLine* curr_beachLine = static_cast<BeachLine*>(curr_event->ptr);
            int parabola_index = std::distance(
                this->beachLine.begin(),
                std::find_if(this->beachLine.begin(), this->beachLine.end(),
                    [curr_beachLine](const BeachLine& bl) { return bl.id == curr_beachLine->id; }
                )
            );

            Site* prev_parabola = static_cast<Site*>(this->beachLine[parabola_index - 2].ptr);
            Edge* edge_left = static_cast<Edge*>(this->beachLine[parabola_index - 1].ptr);
            Site* parabola = static_cast<Site*>(this->beachLine[parabola_index].ptr);
            Edge* edge_right = static_cast<Edge*>(this->beachLine[parabola_index + 1].ptr);
            Site* next_parabola = static_cast<Site*>(this->beachLine[parabola_index + 2].ptr);

            // check if prev_parabola has a circle event then remove that circle event
            // ...

            // check if next_parabola has a circle event then remove that circle event
            // ...

            float ix, iy;
            Line::intersection_ray(
                edge_left->x, edge_left->y, edge_left->dx, edge_left->dy,
                edge_right->x, edge_right->y, edge_right->dx, edge_right->dy,
                &ix, &iy
            );

            // finish edge_left and edge_right at (ix, iy)
            this->finishedHalfEdges.push_back(Edge(edge_left->x, edge_left->y, ix, iy));
            this->finishedHalfEdges.push_back(Edge(edge_right->x, edge_right->y, ix, iy));

            parabola_index--;
            this->beachLine.erase(this->beachLine.begin() + parabola_index); // remove left edge
            this->beachLine.erase(this->beachLine.begin() + parabola_index); // remove parabola
            this->beachLine.erase(this->beachLine.begin() + parabola_index); // remove right edge

            float dx = next_parabola->x - prev_parabola->x;
            float dy = next_parabola->y - prev_parabola->y;

            Edge* new_edge = new Edge(ix, iy, dy, -dx);
            this->beachLine.insert(this->beachLine.begin() + parabola_index, BeachLine(beachLineCounter++, EDGE, new_edge));

            // check and add circle events if needed
            checkCircleEvent(parabola_index - 1);
            checkCircleEvent(parabola_index + 1);
        }

        // remove event
        this->events.erase(this->events.begin());
        count++;
    }
    if (count == 1000) std::cout << "INFINITE" << std::endl; // temporary
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
    if (parabola_index < 2 || parabola_index > this->beachLine.size() - 3) return;

    Edge* edge_left = static_cast<Edge*>(this->beachLine[parabola_index - 1].ptr);
    Site* parabola = static_cast<Site*>(this->beachLine[parabola_index].ptr);
    Edge* edge_right = static_cast<Edge*>(this->beachLine[parabola_index + 1].ptr);

    float ix, iy;
    if (Line::intersection_ray(
            edge_left->x, edge_left->y, edge_left->dx, edge_left->dy,
            edge_right->x, edge_right->y, edge_right->dx, edge_right->dy,
            &ix, &iy)
    ) {
        float dx = parabola->x - ix;
        float dy = parabola->y - iy;
        float r = sqrt(dx * dx + dy * dy);

        // add new circle event
        int index = 0; // can it be 0 ???
        while (index < this->events.size()) {
            if (iy + r < this->events[index].y) break;
            index++;
        }
        this->events.insert(this->events.begin() + index, Event(CIRCLE, iy + r, &this->beachLine[parabola_index]));
    }
}



void Voronoi::visualisation(Canvas* canvas, int sweepLine_y)
{
    // events
    for (int i = 0; i < this->events.size(); i++) {
        Line::draw(canvas, 0, this->events[i].y, canvas->width - 1, this->events[i].y,
            this->events[i].type == SITE ? EGA_RED : EGA_MAGENTA);
    }

    // sites
    for (int i = 0; i < this->siteCount; i++) {
        Circle::draw_filled(canvas, this->sites[i].x, this->sites[i].y, 1, EGA_BRIGHT_RED);
    }

    // sweep line
    Line::draw(canvas, 0, sweepLine_y, canvas->width - 1, sweepLine_y, EGA_GREY);

    if (this->beachLine.empty()) return;

    // draw all full parabolas
    for (int i = 0; i < this->beachLine.size(); i += 2) {
        Site* par = static_cast<Site*>(this->beachLine[i].ptr);
        drawParabola(canvas, par->x, par->y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
    }

    // draw beachline
    float min_x = 0;
    for (int i = 0; i < this->beachLine.size() - 1; i += 2) {
        Site* curr_par = static_cast<Site*>(this->beachLine[i].ptr);
        Site* next_par = static_cast<Site*>(this->beachLine[i + 2].ptr);
        float max_x =
            (i < this->beachLine.size() - 1) ?
                getIntersect_x(curr_par->x, curr_par->y, next_par->x, next_par->y, sweepLine_y) :
                canvas->width - 1;

        // draw parabola
        drawParabola(canvas,curr_par->x, curr_par->y, sweepLine_y, min_x, max_x, EGA_GREEN);

        // draw right halfedge
        Edge* edge_right = static_cast<Edge*>(this->beachLine[i + 1].ptr);
        float slope_right = edge_right->dy / edge_right->dx;
        float dx_right = max_x - edge_right->x;
        Line::draw(canvas, edge_right->x, edge_right->y, edge_right->x + dx_right, edge_right->y + dx_right * slope_right, EGA_CYAN);

        // draw edge start
        canvas->setPixel(edge_right->x, edge_right->y, EGA_WHITE);

        min_x = max_x;
    }

    // draw circle events
    for (int i = 0; i < this->events.size(); i++) {
        if (this->events[i].type != CIRCLE) continue;

        BeachLine* curr_beachLine = static_cast<BeachLine*>(this->events[i].ptr);
        int parabola_index = std::distance(
            this->beachLine.begin(),
            std::find_if(this->beachLine.begin(), this->beachLine.end(),
                [curr_beachLine](const BeachLine& bl) { return bl.id == curr_beachLine->id; }
            )
        );

        Edge* edge_left = static_cast<Edge*>(this->beachLine[parabola_index - 1].ptr);
        Site* parabola = static_cast<Site*>(this->beachLine[parabola_index].ptr);
        Edge* edge_right = static_cast<Edge*>(this->beachLine[parabola_index + 1].ptr);

        // // helpers
        // Line::draw(canvas, edge_left->x, edge_left->y, edge_left->x - edge_left->dx, edge_left->y + edge_left->dy, EGA_LIGHT_BLUE);
        // Circle::draw(canvas, edge_left->x, edge_left->y, 2, EGA_LIGHT_BLUE);
        // Line::draw(canvas, edge_right->x, edge_right->y, edge_right->x - edge_right->dx, edge_right->y - edge_right->dy, EGA_ORANGE);
        // Circle::draw(canvas, edge_right->x, edge_right->y, 2, EGA_ORANGE);

        float ix, iy;
        if (Line::intersection_ray(
                edge_left->x, edge_left->y, edge_left->dx, edge_left->dy,
                edge_right->x, edge_right->y, edge_right->dx, edge_right->dy,
                &ix, &iy)
        ) {
            float dx = parabola->x - ix;
            float dy = parabola->y - iy;
            float r = sqrt(dx * dx + dy * dy);
            Circle::draw(canvas, ix, iy, r, EGA_MAGENTA);
            Circle::draw(canvas, ix, iy, 1, EGA_MAGENTA);
        }
    }

    // draw finished edges
    for (int i = 0; i < this->finishedHalfEdges.size(); i++) {
        Line::draw(canvas,
            this->finishedHalfEdges[i].x, this->finishedHalfEdges[i].y,
            this->finishedHalfEdges[i].dx, this->finishedHalfEdges[i].dy,
            EGA_YELLOW
        );
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
