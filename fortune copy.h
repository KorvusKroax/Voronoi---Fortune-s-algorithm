#pragma once

#include <deque>
#include <algorithm> // for std::sort
#include <cmath> // for std::sqrt
#include <memory> // for std::unique_ptr
#include <iostream> // for std::cout and std::endl

#include "canvas.h" // for visualization purposes
#include "line.h"
#include "circle.h"



struct Edge {
    double x1, y1, x2, y2;

    Edge(double x1, double y1, double x2, double y2) : x1(x1), y1(y1), x2(x2), y2(y2) { }
};



struct Site
{
    double x, y;
    std::deque<Edge> edges;

    Site() : x(0), y(0) { this->edges.clear(); }

    Site(double x, double y) : x(x), y(y) { this->edges.clear(); }

    void addEdge(double x1, double y1, double x2, double y2)
    {
        this->edges.push_back(
            cross(x1, y1, x2, y2, x, y) < 0 ?
                Edge(x1, y1, x2, y2) :
                Edge(x2, y2, x1, y1)
        );
    }

    inline double cross(double x1, double y1, double x2, double y2, double px, double py)
    {
        return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
        // result > 0: p is left
        // result = 0: p is on line
        // result < 0: p is right
    }
};



struct Event
{
    enum Type { SITE, CIRCLE };

    Type type;
    double x;
    double y;
    void* ptr;
        // SITE: Site*
        // CIRCLE: Beachline*

    Event(Type type, double x, double y, void* ptr) : type(type), x(x), y(y), ptr(ptr) { }
};



struct Beachline
{
    enum Type { PARABOLA, HALF_EDGE };

    Type type;
    void* ptr;
        // PARABOLA: Site*
        // HALF_EDGE: HalfEdge*

    Beachline(Type type, void* ptr) : type(type), ptr(ptr) { }
};



struct HalfEdge
{
    double x;
    double y;
    double dir_x;
    double dir_y;

    HalfEdge(double x, double y, double dir_x, double dir_y) : x(x), y(y), dir_x(dir_x), dir_y(dir_y) { }
};



class Fortune
{
    public:
        Fortune() { }

        void create(double width, double height, int siteCount, Site* sites, int sweepLine_y)
        {
            this->width = width;
            this->height = height;
            this->siteCount = siteCount;
            this->sites = sites;

            init();

            if (events.front().y > sweepLine_y) return;

            this->beachline.push_back(
                std::make_unique<Beachline>(
                    Beachline(Beachline::PARABOLA, static_cast<Site*>(events.front().ptr))
                )
            );
            this->events.pop_front();

            while (!this->events.empty()) {
                Event event = this->events.front();

                if (event.y > sweepLine_y) break;

                if (event.type == Event::SITE) {
                    handle_siteEvent(&event);
                } else if (event.type == Event::CIRCLE) {
                    handle_circleEvent(&event);
                }

                this->events.pop_front();
            }
        }

        void init()
        {
            this->events.clear();

            for (int i = 0; i < siteCount; i++) {
                this->events.emplace_back(Event::SITE, sites[i].x, sites[i].y, &sites[i]);
            }

            std::sort(this->events.begin(), this->events.end(),
                [](const Event& a, const Event& b) {
                    return a.y < b.y || (a.y == b.y && a.x < b.x);
                }
            );

            this->beachline.clear();
        }

        void handle_siteEvent(Event* event)
        {
            Site* new_site = static_cast<Site*>(event->ptr);

            int parabola_below_index = getParabolaIndexBelow(new_site);
            Site* parabola_below = static_cast<Site*>(this->beachline[parabola_below_index]->ptr);

            checkCircleEvent_remove(parabola_below_index);

            this->beachline.insert(this->beachline.begin() + parabola_below_index,
                std::make_unique<Beachline>(Beachline::PARABOLA, parabola_below)
            );

            double dx = parabola_below->x - new_site->x;
            double dy = parabola_below->y - new_site->y;
            double edge_origin_y = ((dx * dx) / (dy * 2.0)) + ((parabola_below->y + new_site->y) / 2.0);

            HalfEdge* edge_left = new HalfEdge(new_site->x, edge_origin_y, dy, -dx);
            HalfEdge* edge_right = new HalfEdge(new_site->x, edge_origin_y, -dy, dx);

            this->beachline.insert(this->beachline.begin() + parabola_below_index + 1, std::make_unique<Beachline>(Beachline::HALF_EDGE, edge_left));
            this->beachline.insert(this->beachline.begin() + parabola_below_index + 2, std::make_unique<Beachline>(Beachline::PARABOLA, new_site));
            this->beachline.insert(this->beachline.begin() + parabola_below_index + 3, std::make_unique<Beachline>(Beachline::HALF_EDGE, edge_right));

            checkCircleEvent_add(parabola_below_index);
            checkCircleEvent_add(parabola_below_index + 4);
        }

        void handle_circleEvent(Event* event)
        {
            int parabola_index = -1;
            for (int i = 0; i < this->beachline.size(); i += 2) {
                if (event->ptr == this->beachline[i].get()) {
                    parabola_index = i;
                    break;
                }
            }

            if (parabola_index < 0 || parabola_index >= this->beachline.size()) {
                std::cerr << "Error: parabola not found in handle_circleEvent() (" << parabola_index << ")" << std::endl;
                exit(EXIT_FAILURE);
            }

            checkCircleEvent_remove(parabola_index - 2);
            checkCircleEvent_remove(parabola_index + 2);

            Site* parabola_left = static_cast<Site*>(this->beachline[parabola_index - 2]->ptr);
            Site* parabola = static_cast<Site*>(this->beachline[parabola_index]->ptr);
            Site* parabola_right = static_cast<Site*>(this->beachline[parabola_index + 2]->ptr);

            HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1]->ptr);
            HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1]->ptr);

            double ix, iy, r;
            if (!checkCircle_edge(parabola_index, &ix, &iy, &r)) {
                std::cerr << "Error: Invalid parabola index in handle_circleEvent() (" << parabola_index << ")" << std::endl;
                exit(EXIT_FAILURE);
            }

            parabola_left->addEdge(edge_left->x, edge_left->y, ix, iy);
            parabola->addEdge(edge_left->x, edge_left->y, ix, iy);
            parabola->addEdge(edge_right->x, edge_right->y, ix, iy);
            parabola_right->addEdge(edge_right->x, edge_right->y, ix, iy);

            parabola_index--;
            this->beachline.erase(this->beachline.begin() + parabola_index); // remove left edge
            this->beachline.erase(this->beachline.begin() + parabola_index); // remove parabola
            this->beachline.erase(this->beachline.begin() + parabola_index); // remove right edge

            float dx = parabola_right->x - parabola_left->x;
            float dy = parabola_right->y - parabola_left->y;
            HalfEdge* new_edge = new HalfEdge(ix, iy, -dy, dx);
            this->beachline.insert(this->beachline.begin() + parabola_index,
                std::make_unique<Beachline>(Beachline::HALF_EDGE, new_edge)
            );

            checkCircleEvent_add(parabola_index - 1);
            checkCircleEvent_add(parabola_index + 1);
        }

        int getParabolaIndexBelow(Site* site)
        {
            for (int i = 0; i < this->beachline.size() - 1; i += 2) {
                Site* parabola = static_cast<Site*>(this->beachline[i]->ptr);
                Site* parabola_next = static_cast<Site*>(this->beachline[i + 2]->ptr);
                double intersect_x = getParabolaIntersectX(
                    parabola->x, parabola->y,
                    parabola_next->x, parabola_next->y,
                    site->y
                );
                if (site->x < intersect_x) return i;
            }

            return this->beachline.size() - 1;
        }

        double getParabolaIntersectX(double focus_x, double focus_y, double nextFocus_x, double nextFocus_y, double sweepLine_y)
        {
            if (focus_y == sweepLine_y) return focus_x;
            if (nextFocus_y == sweepLine_y) return nextFocus_x;

            if (focus_y == nextFocus_y) return (focus_x + nextFocus_x) / 2.0;

            double dp = 2.0 * (focus_y - sweepLine_y);
            double a1 = 1.0 / dp;
            double b1 = -2.0 * focus_x / dp;
            double c1 = sweepLine_y + dp / 4.0 + (focus_x * focus_x) / dp;

            dp = 2.0 * (nextFocus_y - sweepLine_y);
            double a2 = 1.0 / dp;
            double b2 = -2.0 * nextFocus_x / dp;
            double c2 = sweepLine_y + dp / 4.0 + (nextFocus_x * nextFocus_x) / dp;

            double a = a1 - a2;
            double b = b1 - b2;
            double c = c1 - c2;

            double sqrtDiscriminant = std::sqrt(b * b - 4.0 * a * c);
            double x1 = (-b + sqrtDiscriminant) / (2.0 * a);
            double x2 = (-b - sqrtDiscriminant) / (2.0 * a);

            return focus_y > nextFocus_y ? std::max(x1, x2) : std::min(x1, x2);
        }

        void checkCircleEvent_remove(int parabola_index)
        {
            for (int i = 0; i < this->events.size(); i++) {
                if (this->events[i].type == Event::CIRCLE && this->events[i].ptr == this->beachline[parabola_index].get()) {
                    this->events.erase(this->events.begin() + i);
                    break;
                }
            }
        }

        void checkCircleEvent_add(int parabola_index)
        {
            if (parabola_index - 2 < 0 || parabola_index + 2 > this->beachline.size() - 1 ||
                this->beachline[parabola_index - 2]->ptr == this->beachline[parabola_index + 2]->ptr) return;

            double ix, iy, r;
            if (!checkCircle_edge(parabola_index, &ix, &iy, &r)) return;

            insertEvent(Event(Event::CIRCLE, ix, iy + r, this->beachline[parabola_index].get()));
        }

        bool checkCircle_edge(int parabola_index, double* ix, double* iy, double* r)
        {
            HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1]->ptr);
            Site* parabola = static_cast<Site*>(this->beachline[parabola_index]->ptr);
            HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1]->ptr);

            if (!intersection_ray(
                    edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
                    edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
                    ix, iy
                )
            ) return false;

            double dx = parabola->x - *ix;
            double dy = parabola->y - *iy;
            *r = std::sqrt(dx * dx + dy * dy);

            return true;
        }

        bool intersection_ray(double x1, double y1, double x1_dir, double y1_dir, double x2, double y2, double x2_dir, double y2_dir, double* ix, double* iy)
        {
            double den = x1_dir * y2_dir - y1_dir * x2_dir;
            if (den == 0) return false;

            double t = ((x2 - x1) * y2_dir - (y2 - y1) * x2_dir) / den;
            if (0 > t) return false;

            double u = (x1_dir * (y2 - y1) - y1_dir * (x2 - x1)) / -den;
            if (0 > u) return false;

            *ix = x1 + x1_dir * t;
            *iy = y1 + y1_dir * t;
            return true;
        }

        void insertEvent(Event event)
        {
            if (this->events.empty() || event.y < this->events.front().y) {
                this->events.push_front(event);
                return;
            }

            if (event.y > this->events.back().y) {
                this->events.push_back(event);
                return;
            }

            int index = 0;
            while (index < this->events.size() - 1) {
                if (event.y < this->events[index].y) break;
                index++;
            }

            this->events.insert(this->events.begin() + index, event);
        }

    private:
        double width;
        double height;
        int siteCount;
        Site* sites;

        std::deque<Event> events;
        std::deque<std::unique_ptr<Beachline>> beachline;





    public:
        void visualisation(Canvas* canvas, float sweepLine_y)
        {
            // events
            for (int i = 0; i < this->events.size(); i++) {
                Line::draw(canvas, 0, this->events[i].y, canvas->width - 1, this->events[i].y,
                    this->events[i].type == Event::SITE ? EGA_RED : EGA_MAGENTA);
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
                Site* par = static_cast<Site*>(this->beachline[i]->ptr);
                if (par->y > sweepLine_y) continue;
                drawParabola(canvas, par->x, par->y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
            }

            // draw beachline
            float min_x = 0;
            for (int i = 0; i < this->beachline.size() - 1; i += 2) {
                Site* curr_par = static_cast<Site*>(this->beachline[i]->ptr);
                Site* next_par = static_cast<Site*>(this->beachline[i + 2]->ptr);

                float max_x = (i < this->beachline.size() - 1) ?
                    getParabolaIntersectX(curr_par->x, curr_par->y, next_par->x, next_par->y, sweepLine_y) :
                    canvas->width - 1;

                // draw parabola
                drawParabola(canvas, curr_par->x, curr_par->y, sweepLine_y, min_x, max_x, EGA_GREEN);

                // draw right halfedge
                HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[i + 1]->ptr);
                float slope_right = edge_right->dir_y / edge_right->dir_x;
                float dx_right = max_x - edge_right->x;
                Line::draw(canvas, edge_right->x, edge_right->y, edge_right->x + dx_right, edge_right->y + dx_right * slope_right, EGA_CYAN);

                // draw edge start
                canvas->setPixel(edge_right->x, edge_right->y, EGA_WHITE);

                min_x = max_x;
            }

            // // draw circle events
            // for (int i = 0; i < this->events.size(); i++) {
            //     if (this->events[i].type != Event::CIRCLE) continue;

            //     int parabola_index = -1;
            //     for (int j = 0; j < this->beachline.size(); j += 2) {
            //         if (this->events[i].ptr == this->beachline[j].get()) {
            //             parabola_index = j;
            //             break;
            //         }
            //     }

            //     HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1]->ptr);
            //     Site* parabola = static_cast<Site*>(this->beachline[parabola_index]->ptr);
            //     HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1]->ptr);

            //     // float ix, iy;
            //     // if (Line::intersection_ray(
            //     //         edge_left->x, edge_left->y, edge_left->dir_x, edge_left->dir_y,
            //     //         edge_right->x, edge_right->y, edge_right->dir_x, edge_right->dir_y,
            //     //         &ix, &iy
            //     //     )
            //     // ) {
            //     //     float dx = parabola->x - ix;
            //     //     float dy = parabola->y - iy;
            //     //     float r = sqrt(dx * dx + dy * dy);
            //     //     Circle::draw(canvas, ix, iy, r, EGA_MAGENTA);
            //     //     Circle::draw(canvas, ix, iy, 1, EGA_BRIGHT_MAGENTA);
            //     // }
            // }

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

        void drawParabola(Canvas* canvas, float focus_x, float focus_y, float sweepLine_y, float min_x, float max_x, Color color)
        {
            if (focus_y == sweepLine_y) return;
            for (int x = min_x; x < max_x; x++) {
                // int y = (1.0f / (2 * (focus_y - sweepLine_y))) * ((x - focus_x) * (x - focus_x)) + ((focus_y + sweepLine_y) / 2);
                int y = (((x - focus_x) * (x - focus_x)) / (2 * (focus_y - sweepLine_y))) + ((focus_y + sweepLine_y) / 2);
                canvas->setPixel(x, y, color);
            }
        }

};
