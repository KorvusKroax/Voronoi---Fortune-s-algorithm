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



void Voronoi::create()
{
    init();

    Event* curr_event = &this->events.front();

    // add first parabola in the beachline
    this->beachline.push_back(Beachline(beachline_id++, PARABOLA, curr_event->ptr));
    this->events.erase(this->events.begin());

    while (!this->events.empty()) {
        curr_event = &this->events.front();
        if (curr_event->type == SITE) siteEvent(curr_event);
        else if (curr_event->type == CIRCLE) circleEvent(curr_event);

        // remove event
        this->events.erase(this->events.begin());
    }

    finishingHalfEdges();
}

void Voronoi::create(int sweepLine_y)
{
    init();

    // get first event (site)
    Event* curr_event = &this->events.front();
    if (curr_event->y > sweepLine_y) return; // temporary

    // add first parabola in the beachline
    this->beachline.push_back(Beachline(beachline_id++, PARABOLA, curr_event->ptr));
    this->events.erase(this->events.begin());

    while (!this->events.empty()) {
        curr_event = &this->events.front();

        if (curr_event->y > sweepLine_y) break; // temporary

        if (curr_event->type == SITE) siteEvent(curr_event);
        else if (curr_event->type == CIRCLE) circleEvent(curr_event);

        // remove event
        this->events.erase(this->events.begin());
    }

    if (this->events.empty()) finishingHalfEdges();
}

void Voronoi::init()
{
    this->events.clear();
    this->beachline.clear();
    beachline_id = 0;

    this->finishedHalfEdges.clear();

    // set events (add site events)
    for (int i = 0; i < this->siteCount; i++) {
        this->events.push_back(Event(SITE, this->sites[i].y, &this->sites[i]));
    }
    // sort events by y
    std::sort(this->events.begin(), this->events.end(),
        [](const Event& lhs, const Event& rhs) -> bool { return lhs.y < rhs.y; }
    );
}



void Voronoi::siteEvent(Event* curr_event)
{
    Site* curr_event_site = static_cast<Site*>(curr_event->ptr);

    int index = getBeachlineIndexBelow(curr_event_site);

    Site* parabola_below = static_cast<Site*>(this->beachline[index].ptr);

    checkAndRemoveCircleEvent(index);

    // duplicate the parabola_below
    this->beachline.insert(this->beachline.begin() + index, Beachline(beachline_id++, PARABOLA, parabola_below));

    // insert new parabola with two edges beside it
    float dx = parabola_below->x - curr_event_site->x;
    float dy = parabola_below->y - curr_event_site->y;
    float edgeStart_y = (float(dx * dx) / (2 * dy)) + (float(parabola_below->y + curr_event_site->y) / 2);

    HalfEdge* edge_left = new HalfEdge(curr_event_site->x, edgeStart_y, dy, -dx);
    this->beachline.insert(this->beachline.begin() + index + 1, Beachline(beachline_id++, EDGE, edge_left));

    this->beachline.insert(this->beachline.begin() + index + 2, Beachline(beachline_id++, PARABOLA, curr_event->ptr));

    HalfEdge* edge_right = new HalfEdge(curr_event_site->x, edgeStart_y, -dy, dx);
    this->beachline.insert(this->beachline.begin() + index + 3, Beachline(beachline_id++, EDGE, edge_right));

    // check and add circle events if needed
    checkAndAddCircleEvent(index);
    checkAndAddCircleEvent(index + 4);
}

int Voronoi::getBeachlineIndexBelow(Site* curr_event_site)
{
    for (int i = 0; i < this->beachline.size() - 1; i += 2) {
        Site* curr_par = static_cast<Site*>(beachline[i].ptr);
        Site* next_par = static_cast<Site*>(beachline[i + 2].ptr);
        float intersect_x = getParabolaIntersect_x(
            curr_par->x, curr_par->y,
            next_par->x, next_par->y,
            curr_event_site->y
        );
        if (curr_event_site->x < intersect_x) return i;
    }
    return 0;
}



void Voronoi::circleEvent(Event* curr_event)
{
    Beachline* curr_beachline = static_cast<Beachline*>(curr_event->ptr);
    int parabola_index = std::distance(
        this->beachline.begin(),
        std::find_if(this->beachline.begin(), this->beachline.end(),
            [curr_beachline](const Beachline& bl) { return bl.id == curr_beachline->id; }
        )
    );

    Site* prev_parabola = static_cast<Site*>(this->beachline[parabola_index - 2].ptr);
    HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1].ptr);
    Site* parabola = static_cast<Site*>(this->beachline[parabola_index].ptr);
    HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1].ptr);
    Site* next_parabola = static_cast<Site*>(this->beachline[parabola_index + 2].ptr);

    checkAndRemoveCircleEvent(parabola_index - 2);
    checkAndRemoveCircleEvent(parabola_index + 2);

    float ix, iy;
    Line::intersection_ray(
        edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
        edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
        &ix, &iy
    );

    // finish edge_left and edge_right at (ix, iy)
    this->finishedHalfEdges.push_back(HalfEdge(edge_left->x, edge_left->y, ix, iy));
    this->finishedHalfEdges.push_back(HalfEdge(edge_right->x, edge_right->y, ix, iy));

    parabola_index--;
    this->beachline.erase(this->beachline.begin() + parabola_index); // remove left edge
    this->beachline.erase(this->beachline.begin() + parabola_index); // remove parabola
    this->beachline.erase(this->beachline.begin() + parabola_index); // remove right edge

    float dx = next_parabola->x - prev_parabola->x;
    float dy = next_parabola->y - prev_parabola->y;

    HalfEdge* new_edge = new HalfEdge(ix, iy, -dy, dx);
    this->beachline.insert(this->beachline.begin() + parabola_index, Beachline(beachline_id++, EDGE, new_edge));

    // check and add circle events if needed
    checkAndAddCircleEvent(parabola_index - 1);
    checkAndAddCircleEvent(parabola_index + 1);
}



double Voronoi::getParabolaIntersect_x(double site_x, double site_y, double nextSite_x, double nextSite_y, double sweepLine_y)
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

void Voronoi::checkAndAddCircleEvent(int parabola_index)
{
    if (parabola_index < 2 || parabola_index > this->beachline.size() - 3) return;

    HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1].ptr);
    Site* parabola = static_cast<Site*>(this->beachline[parabola_index].ptr);
    HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1].ptr);

    float ix, iy;
    if (Line::intersection_ray(
            edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
            edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
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
        this->events.insert(this->events.begin() + index, Event(CIRCLE, iy + r, &this->beachline[parabola_index]));
    }
}

void Voronoi::checkAndRemoveCircleEvent(int parabola_index)
{
    for (int i = 0; i < this->events.size(); i++) {
        if (this->events[i].type == CIRCLE && this->events[i].ptr == &this->beachline[parabola_index]) {
            this->events.erase(this->events.begin() + i);
            break;
        }
    }
}

void Voronoi::finishingHalfEdges()
{
    for (int i = 0; i < this->beachline.size(); i++) {
        if (this->beachline[i].type == EDGE) {
            HalfEdge* edge = static_cast<HalfEdge*>(this->beachline[i].ptr);

            float end_x, end_y;
            if (abs(edge->dir_x) > abs(edge->dir_y)) {
                float length_x = edge->dir_x < 0 ? -edge->x : this->width - edge->x - 1;
                end_x = edge->x + length_x;
                end_y = edge->y + length_x * (edge->dir_y / edge->dir_x);
            } else {
                float length_y = edge->dir_y < 0 ? -edge->y : this->height - edge->y - 1;
                end_x = edge->x + length_y * (edge->dir_x / edge->dir_y);
                end_y = edge->y + length_y;
            }
            this->finishedHalfEdges.push_back(HalfEdge(edge->x, edge->y, end_x, end_y));
        }
    }
}



void Voronoi::visualisation(Canvas* canvas)
{
    // sites
    for (int i = 0; i < this->siteCount; i++) {
        Circle::draw_filled(canvas, this->sites[i].x, this->sites[i].y, 1, EGA_BRIGHT_RED);
    }

    // draw finished edges
    for (int i = 0; i < this->finishedHalfEdges.size(); i++) {
        Line::draw(canvas,
            this->finishedHalfEdges[i].x, this->finishedHalfEdges[i].y,
            this->finishedHalfEdges[i].dir_x, this->finishedHalfEdges[i].dir_y,
            EGA_YELLOW
        );
        Circle::draw(canvas, this->finishedHalfEdges[i].x, this->finishedHalfEdges[i].y, 2, EGA_GREEN);
        Circle::draw(canvas, this->finishedHalfEdges[i].dir_x, this->finishedHalfEdges[i].dir_y, 2, EGA_RED);
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

    if (this->beachline.empty()) return;

    // draw all full parabolas
    for (int i = 0; i < this->beachline.size(); i += 2) {
        Site* par = static_cast<Site*>(this->beachline[i].ptr);
        drawParabola(canvas, par->x, par->y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
    }

    // draw beachline
    float min_x = 0;
    for (int i = 0; i < this->beachline.size() - 1; i += 2) {
        Site* curr_par = static_cast<Site*>(this->beachline[i].ptr);
        Site* next_par = static_cast<Site*>(this->beachline[i + 2].ptr);
        float max_x =
            (i < this->beachline.size() - 1) ?
                getParabolaIntersect_x(curr_par->x, curr_par->y, next_par->x, next_par->y, sweepLine_y) :
                canvas->width - 1;

        // draw parabola
        drawParabola(canvas,curr_par->x, curr_par->y, sweepLine_y, min_x, max_x, EGA_GREEN);

        // draw right halfedge
        HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[i + 1].ptr);
        float slope_right = edge_right->dir_y / edge_right->dir_x;
        float dx_right = max_x - edge_right->x;
        Line::draw(canvas, edge_right->x, edge_right->y, edge_right->x + dx_right, edge_right->y + dx_right * slope_right, EGA_CYAN);

        // draw edge start
        canvas->setPixel(edge_right->x, edge_right->y, EGA_WHITE);

        min_x = max_x;
    }

    // draw circle events
    for (int i = 0; i < this->events.size(); i++) {
        if (this->events[i].type != CIRCLE) continue;

        Beachline* curr_beachline = static_cast<Beachline*>(this->events[i].ptr);
        int parabola_index = std::distance(
            this->beachline.begin(),
            std::find_if(this->beachline.begin(), this->beachline.end(),
                [curr_beachline](const Beachline& bl) { return bl.id == curr_beachline->id; }
            )
        );

        HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1].ptr);
        Site* parabola = static_cast<Site*>(this->beachline[parabola_index].ptr);
        HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1].ptr);

        float ix, iy;
        if (Line::intersection_ray(
                edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
                edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
                &ix, &iy)
        ) {
            float dx = parabola->x - ix;
            float dy = parabola->y - iy;
            float r = sqrt(dx * dx + dy * dy);
            Circle::draw(canvas, ix, iy, r, EGA_MAGENTA);
            Circle::draw(canvas, ix, iy, 1, EGA_BRIGHT_MAGENTA);
        }
    }

    // draw finished edges
    for (int i = 0; i < this->finishedHalfEdges.size(); i++) {
        Line::draw(canvas,
            this->finishedHalfEdges[i].x, this->finishedHalfEdges[i].y,
            this->finishedHalfEdges[i].dir_x, this->finishedHalfEdges[i].dir_y,
            EGA_YELLOW
        );
        Circle::draw(canvas, this->finishedHalfEdges[i].x, this->finishedHalfEdges[i].y, 2, EGA_GREEN);
        Circle::draw(canvas, this->finishedHalfEdges[i].dir_x, this->finishedHalfEdges[i].dir_y, 2, EGA_RED);
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
