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

void Voronoi::init()
{
    this->events.clear();
    this->beachline.clear();
    this->beachline_id = 0;

    // set events (add site events)
    for (int i = 0; i < this->siteCount; i++) {
        this->events.push_back(Event(SITE, this->sites[i].x, this->sites[i].y, &this->sites[i]));
    }

    // sort events by y (and x if y is the same)
    std::sort(this->events.begin(), this->events.end());
}

void Voronoi::create()
{
    init();

    Event* curr_event = &this->events.front();

    // add first parabola in the beachline
    this->beachline.push_back(Beachline(this->beachline_id++, PARABOLA, curr_event->ptr));
    this->events.erase(this->events.begin());

    int count = 0;
    while (!this->events.empty()) {
        curr_event = &this->events.front();
        std::cout << "START " << count << std::endl;

        if (curr_event->type == SITE) siteEvent(curr_event);
        else if (curr_event->type == CIRCLE) circleEvent(curr_event);

        // remove event
        this->events.erase(this->events.begin());

        std::cout << "DONE" << std::endl;
        std::cout << std::endl;
        count++;
    }

    finishingHalfEdges();
}

/*
void Voronoi::create(int sweepLine_y)
{
    init();

    // get first event (site)
    Event* curr_event = &this->events.front();
    if (curr_event->y > sweepLine_y) return;

    // add first parabola in the beachline
    this->beachline.push_back(Beachline(this->beachline_id++, PARABOLA, curr_event->ptr));
    this->events.erase(this->events.begin());

    while (!this->events.empty()) {
        curr_event = &this->events.front();
        if (curr_event->y > sweepLine_y) break;

        if (curr_event->type == SITE) siteEvent(curr_event);
        else if (curr_event->type == CIRCLE) circleEvent(curr_event);

        // remove event
        this->events.erase(this->events.begin());
    }

    if (this->events.empty()) finishingHalfEdges();
}
*/

void Voronoi::siteEvent(Event* curr_event)
{
    std::cout << "siteEvent()" << std::endl;

    Site* curr_event_site = static_cast<Site*>(curr_event->ptr);

    int index = getParabolaIndexBelow(curr_event_site);

    checkAndRemoveCircleEvent(index);

    Site* parabola_below = static_cast<Site*>(this->beachline[index].ptr);

    // duplicate the parabola_below
    this->beachline.insert(this->beachline.begin() + index, Beachline(this->beachline_id++, PARABOLA, parabola_below));

    // insert new parabola with two edges beside it
    float dx = parabola_below->x - curr_event_site->x;
    float dy = parabola_below->y - curr_event_site->y;
    float edgeStart_y = ((dx * dx) / (dy * 2)) + ((parabola_below->y + curr_event_site->y) / 2);

    HalfEdge* edge_left = new HalfEdge(curr_event_site->x, edgeStart_y, dy, -dx);
    HalfEdge* edge_right = new HalfEdge(curr_event_site->x, edgeStart_y, -dy, dx);

    this->beachline.insert(this->beachline.begin() + index + 1, Beachline(this->beachline_id++, EDGE, edge_left));
    this->beachline.insert(this->beachline.begin() + index + 2, Beachline(this->beachline_id++, PARABOLA, curr_event->ptr));
    this->beachline.insert(this->beachline.begin() + index + 3, Beachline(this->beachline_id++, EDGE, edge_right));

    // check and add circle events if needed
    checkAndAddCircleEvent(index);
    checkAndAddCircleEvent(index + 4);

    // if (index > 1) checkAndAddCircleEvent_circle(index);
    // if (index + 4 < this->beachline.size() - 2) checkAndAddCircleEvent_circle(index + 4);
}

void Voronoi::circleEvent(Event* curr_event)
{
    std::cout << "circleEvent()" << std::endl;

    Beachline* curr_beachline = static_cast<Beachline*>(curr_event->ptr);
    int index = std::distance(
        this->beachline.begin(),
        std::find_if(this->beachline.begin(), this->beachline.end(),
            [curr_beachline](const Beachline& bl) { return bl.id == curr_beachline->id; }
        )
    );

    std::cout << "CIRCLE EVENT INDEX: " << index << std::endl;

    checkAndRemoveCircleEvent(index - 2);
    checkAndRemoveCircleEvent(index + 2);

    Site* prev_parabola = static_cast<Site*>(this->beachline[index - 2].ptr);
    HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[index - 1].ptr);
    Site* parabola = static_cast<Site*>(this->beachline[index].ptr);
    HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[index + 1].ptr);
    Site* next_parabola = static_cast<Site*>(this->beachline[index + 2].ptr);

    float ix, iy;
    Line::intersection_ray(
        edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
        edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
        &ix, &iy
    );

    // finish edge_left and edge_right at (ix, iy)
    prev_parabola->addEdge(edge_left->x, edge_left->y, ix, iy);
    parabola->addEdge(edge_left->x, edge_left->y, ix, iy);
    parabola->addEdge(edge_right->x, edge_right->y, ix, iy);
    next_parabola->addEdge(edge_right->x, edge_right->y, ix, iy);

    index--;
    this->beachline.erase(this->beachline.begin() + index); // remove left edge
    this->beachline.erase(this->beachline.begin() + index); // remove parabola
    this->beachline.erase(this->beachline.begin() + index); // remove right edge

    float dx = next_parabola->x - prev_parabola->x;
    float dy = next_parabola->y - prev_parabola->y;
    HalfEdge* new_edge = new HalfEdge(ix, iy, -dy, dx);
    this->beachline.insert(this->beachline.begin() + index, Beachline(this->beachline_id++, EDGE, new_edge));

    // check and add circle events if needed
    checkAndAddCircleEvent(index - 1);
    checkAndAddCircleEvent(index + 1);

    // if (index - 3 > 1) checkAndAddCircleEvent_circle(index - 3);
    // if (index + 3 < this->beachline.size() - 2) checkAndAddCircleEvent_circle(index + 3);
}

void Voronoi::checkAndAddCircleEvent(int parabola_index)
{
    std::cout << "checkAndAddCircleEvent()" << std::endl;

    if (parabola_index - 2 < 0 || parabola_index + 2 > this->beachline.size() - 1 ||
        this->beachline[parabola_index - 2].ptr == this->beachline[parabola_index + 2].ptr) return;

    HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1].ptr);
    Site* parabola = static_cast<Site*>(this->beachline[parabola_index].ptr);
    HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1].ptr);

    float ix, iy;
    if (!Line::intersection_ray(
            edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
            edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
            &ix, &iy
        )
    ) return;

    float dx = parabola->x - ix;
    float dy = parabola->y - iy;
    float r = sqrt(dx * dx + dy * dy);

    // if (iy + r < this->events[0].y) return;

    // insert new circle event
    int index = 0; // can it be 0 ???
    while (index < this->events.size()) {
        if (iy + r < this->events[index].y) break;
        index++;
    }

    this->events.insert(this->events.begin() + index, Event(CIRCLE, ix, iy + r, &this->beachline[parabola_index]));

    // !!! ha a beachline mérete változik akkor ez a pointer már nem jó helyre fog mutatni !!!



}

/*
void Voronoi::checkAndAddCircleEvent_circle(int parabola_index)
{
    Site* left_site = static_cast<Site*>(this->beachline[parabola_index - 2].ptr);
    Site* mid_site = static_cast<Site*>(this->beachline[parabola_index].ptr);
    Site* right_site = static_cast<Site*>(this->beachline[parabola_index + 2].ptr);

    float cx, cy, r;
    if (Circle::getCircle(
            left_site->x, left_site->y,
            mid_site->x, mid_site->y,
            right_site->x, right_site->y,
            &cx, &cy, &r
        )
    ) {
        float dx = mid_site->x - cx;
        float dy = mid_site->y - cy;

        // add new circle event
        int index = 0; // can it be 0 ???
        while (index < this->events.size()) {
            if (cy + r < this->events[index].y) break;
            index++;
        }
        this->events.insert(this->events.begin() + index, Event(CIRCLE, cy + r, &this->beachline[index]));
    }
}
*/

void Voronoi::checkAndRemoveCircleEvent(int parabola_index)
{
    std::cout << "checkAndRemoveCircleEvent() - parabola_index: " << parabola_index << ", this->beachline.size(): " << this->beachline.size() << std::endl;

    for (int i = 0; i < this->events.size(); i++) {
        if (this->events[i].type == CIRCLE && this->events[i].ptr == &this->beachline[parabola_index]) {

            std::cout << "REMOVE CIRCLE EVENT: " << i << std::endl;

            this->events.erase(this->events.begin() + i);
            break;
        }
    }
}

void Voronoi::finishingHalfEdges()
{
    std::cout << "finishingHalfEdges()" << std::endl;

    for (int i = 0; i < this->beachline.size(); i++) {
        if (this->beachline[i].type != EDGE) continue;

        Site* left_site = static_cast<Site*>(this->beachline[i - 1].ptr);
        HalfEdge* edge = static_cast<HalfEdge*>(this->beachline[i].ptr);
        Site* right_site = static_cast<Site*>(this->beachline[i + 1].ptr);

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

        // still can be out of bounds
        // ...

        left_site->addEdge(edge->x, edge->y, end_x, end_y);
        right_site->addEdge(edge->x, edge->y, end_x, end_y);
    }
}

int Voronoi::getParabolaIndexBelow(Site* curr_event_site)
{
    std::cout << "getParabolaIndexBelow()" << std::endl;

    for (int i = 0; i < this->beachline.size() - 1; i += 2) {
        Site* curr_parabola = static_cast<Site*>(beachline[i].ptr);
        Site* next_parabola = static_cast<Site*>(beachline[i + 2].ptr);
        float intersect_x = getParabolaIntersect_x(
            curr_parabola->x, curr_parabola->y,
            next_parabola->x, next_parabola->y,
            curr_event_site->y
        );
        if (curr_event_site->x < intersect_x) return i;
    }
    return 0;
}

float Voronoi::getParabolaIntersect_x(float focus_x, float focus_y, float nextFocus_x, float nextFocus_y, float sweepLine_y)
{
    std::cout << "getParabolaIntersect_x()" << std::endl;

    if (focus_y == sweepLine_y) return focus_x;
    if (nextFocus_y == sweepLine_y) return nextFocus_x;

    float dp = 2 * (focus_y - sweepLine_y);
    float a1 = 1 / dp;
    float b1 = -2 * focus_x / dp;
    float c1 = sweepLine_y + dp / 4 + (focus_x * focus_x) / dp;

          dp = 2 * (nextFocus_y - sweepLine_y);
    float a2 = 1 / dp;
    float b2 = -2 * nextFocus_x / dp;
    float c2 = sweepLine_y + dp / 4 + (nextFocus_x * nextFocus_x) / dp;

    float a = a1 - a2;
    float b = b1 - b2;
    float c = c1 - c2;

    float sqrtDiscriminant = std::sqrt(b * b - 4 * a * c);
    float x1 = (-b + sqrtDiscriminant) / (2 * a);
    float x2 = (-b - sqrtDiscriminant) / (2 * a);

    return focus_y > nextFocus_y ? std::max(x1, x2) : std::min(x1, x2);
}




/*
void Voronoi::visualisation(Canvas* canvas, float sweepLine_y)
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

        // float ix, iy;
        // if (Line::intersection_ray(
        //         edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
        //         edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
        //         &ix, &iy
        //     )
        // ) {
        //     float dx = parabola->x - ix;
        //     float dy = parabola->y - iy;
        //     float r = sqrt(dx * dx + dy * dy);
        //     Circle::draw(canvas, ix, iy, r, EGA_MAGENTA);
        //     Circle::draw(canvas, ix, iy, 1, EGA_BRIGHT_MAGENTA);
        // }
    }

    // draw finished edges
    for (int i = 0; i < this->siteCount; i++) {
        for (int j = 0; j < this->sites[i].edges.size(); j++) {
            Line::draw(canvas,
                this->sites[i].edges[j].x1, this->sites[i].edges[j].y1,
                this->sites[i].edges[j].x2, this->sites[i].edges[j].y2,
                EGA_YELLOW
            );
            Circle::draw_filled(canvas, this->sites[i].edges[j].x1, this->sites[i].edges[j].y1, 2, EGA_GREEN);
            Circle::draw(canvas, this->sites[i].edges[j].x2, this->sites[i].edges[j].y2, 4, EGA_RED);
        }
    }
}

void Voronoi::drawParabola(Canvas* canvas, float focus_x, float focus_y, float sweepLine_y, float min_x, float max_x, Color color)
{
    if (focus_y == sweepLine_y) return;
    for (int x = min_x; x < max_x; x++) {
        // int y = (1.0f / (2 * (focus_y - sweepLine_y))) * ((x - focus_x) * (x - focus_x)) + ((focus_y + sweepLine_y) / 2);
        int y = (((x - focus_x) * (x - focus_x)) / (2 * (focus_y - sweepLine_y))) + ((focus_y + sweepLine_y) / 2);
        canvas->setPixel(x, y, color);
    }
}
*/
