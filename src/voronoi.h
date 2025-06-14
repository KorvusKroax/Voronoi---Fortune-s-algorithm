#pragma once

#include <vector>
#include <set>
#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>

struct Point
{
    double x, y;
};

struct Edge
{
    Point start, end;
    Edge(Point s, Point e) : start(s), end(e) {}
    virtual ~Edge() = default;
};

struct HalfEdge : Edge
{
    Point dir;
    HalfEdge* otherHalf = nullptr;
    HalfEdge(Point start, Point direction) : Edge{start, {0, 0}}, dir(direction) {}
};

struct Site
{
    Point point;
    std::vector<Edge*> edges;

    void addEdge(HalfEdge* halfEdge)
    {
        edges.push_back(halfEdge);
    }
};

struct Event
{
    enum Type { SITE, CIRCLE };

    Type type;
    Point point;
    void* ptr; // SITE: Site*, CIRCLE: Beachline*

    bool operator < (const Event& other) const
    {
        return this->point.y < other.point.y || (this->point.y == other.point.y && this->point.x < other.point.x);
    }
};

struct Beachline
{
    enum Type { PARABOLA, HALF_EDGE };

    Type type;
    void* ptr; // PARABOLA: Site*, HALF_EDGE: HalfEdge*
};

struct Voronoi
{
    double width, height;
    size_t siteCount;
    Site* sites;
    std::set<Event> events;
    std::vector<std::unique_ptr<Beachline>> beachline;
    std::vector<HalfEdge*> halfEdges;

    Voronoi(double width, double height, std::vector<Point>* points)
    {
        this->width = width;
        this->height = height;

        this->siteCount = points->size();
        this->sites = new Site[this->siteCount];
        for (size_t i = 0; i < this->siteCount; i++) {
            sites[i].point = (*points)[i];
        }

        this->events.clear();
        for (int i = 0; i < this->siteCount; i++) {
            this->events.insert(Event{Event::SITE, sites[i].point, &sites[i]});
        }

        this->beachline.clear();
        Site* firstSite = static_cast<Site*>(events.begin()->ptr);
        this->beachline.push_back(std::make_unique<Beachline>(Beachline{Beachline::PARABOLA, firstSite}));
        this->events.erase(this->events.begin());

        while (!this->events.empty()) {
            Event event = *this->events.begin();

            if (event.type == Event::SITE) handle_siteEvent(&event);
            else if (event.type == Event::CIRCLE) handle_circleEvent(&event);

            this->events.erase(this->events.begin());
        }

        finishingHalfEdges();
    }

    void handle_siteEvent(Event* event)
    {
        Site* new_site = static_cast<Site*>(event->ptr);

        int parabola_below_index = getParabolaIndexBelow(new_site);
        Site* parabola_below = static_cast<Site*>(this->beachline[parabola_below_index]->ptr);

        checkCircleEvent_remove(parabola_below_index);

        this->beachline.insert(this->beachline.begin() + parabola_below_index,
            std::make_unique<Beachline>(Beachline{Beachline::PARABOLA, parabola_below})
        );

        double dx = parabola_below->point.x - new_site->point.x;
        double dy = parabola_below->point.y - new_site->point.y;
        double edge_start_y = ((dx * dx) / (dy * 2.0)) + ((parabola_below->point.y + new_site->point.y) / 2.0);

        HalfEdge* edge_left = new HalfEdge{{new_site->point.x, edge_start_y}, {dy, -dx}};
        HalfEdge* edge_right = new HalfEdge{{new_site->point.x, edge_start_y}, {-dy, dx}};
        edge_left->otherHalf = edge_right;
        edge_right->otherHalf = edge_left;

        this->halfEdges.push_back(edge_left);
        this->halfEdges.push_back(edge_right);

        this->beachline.insert(this->beachline.begin() + parabola_below_index + 1, std::make_unique<Beachline>(Beachline{Beachline::HALF_EDGE, edge_left}));
        this->beachline.insert(this->beachline.begin() + parabola_below_index + 2, std::make_unique<Beachline>(Beachline{Beachline::PARABOLA, new_site}));
        this->beachline.insert(this->beachline.begin() + parabola_below_index + 3, std::make_unique<Beachline>(Beachline{Beachline::HALF_EDGE, edge_right}));

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

        if (parabola_index < 0 || parabola_index > this->beachline.size() - 1) {
            std::cout << "Error: parabola not found in handle_circleEvent() (" << parabola_index << ")" << std::endl;
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

        if (clipHalfEdge(edge_left, ix, iy)) {
            parabola_left->addEdge(edge_left);
            parabola->addEdge(edge_left);
        }

        if (clipHalfEdge(edge_right, ix, iy)) {
            parabola_right->addEdge(edge_right);
            parabola->addEdge(edge_right);
        }

        parabola_index--;
        this->beachline.erase(this->beachline.begin() + parabola_index); // remove edge_left
        this->beachline.erase(this->beachline.begin() + parabola_index); // remove parabola
        this->beachline.erase(this->beachline.begin() + parabola_index); // remove edge_right

        float dx = parabola_right->point.x - parabola_left->point.x;
        float dy = parabola_right->point.y - parabola_left->point.y;
        HalfEdge* new_edge = new HalfEdge{{ix, iy}, {-dy, dx}};

        this->halfEdges.push_back(new_edge);

        this->beachline.insert(this->beachline.begin() + parabola_index,
            std::make_unique<Beachline>(Beachline{Beachline::HALF_EDGE, new_edge})
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
                parabola->point.x, parabola->point.y,
                parabola_next->point.x, parabola_next->point.y,
                site->point.y
            );
            if (site->point.x < intersect_x) return i;
        }

        return this->beachline.size() - 1;
    }

    double getParabolaIntersectX(double focus_x, double focus_y, double nextFocus_x, double nextFocus_y, double sweepLine_y)
    {
        if (focus_y == sweepLine_y) return focus_x;
        if (nextFocus_y == sweepLine_y) return nextFocus_x;

        if (focus_y == nextFocus_y) return (focus_x + nextFocus_x) / 2.0;

        double dp1 = 2.0 * (focus_y - sweepLine_y);
        double dp2 = 2.0 * (nextFocus_y - sweepLine_y);

        double a = (1.0 / dp1) - (1.0 / dp2);
        double b = (-2.0 * focus_x / dp1) - (-2.0 * nextFocus_x / dp2);
        double c = (sweepLine_y + dp1 / 4.0 + (focus_x * focus_x) / dp1) - (sweepLine_y + dp2 / 4.0 + (nextFocus_x * nextFocus_x) / dp2);

        double sqrtDiscriminant = std::sqrt(b * b - 4.0 * a * c);
        double x1 = (-b + sqrtDiscriminant) / (2.0 * a);
        double x2 = (-b - sqrtDiscriminant) / (2.0 * a);

        return focus_y > nextFocus_y ? std::max(x1, x2) : std::min(x1, x2);
    }



    void checkCircleEvent_remove(int parabola_index)
    {
        Beachline* ptr = this->beachline[parabola_index].get();
        this->events.erase(std::find_if(this->events.begin(), this->events.end(),
            [ptr](const Event& event) { return event.type == Event::CIRCLE && event.ptr == ptr; }
        ));

        // for (int i = 0; i < this->events.size(); i++) {
        //     if (this->events[i].type == Event::CIRCLE && this->events[i].ptr == this->beachline[parabola_index].get()) {
        //         this->events.erase(this->events.begin() + i);
        //         break;
        //     }
        // }
    }

    void checkCircleEvent_add(int parabola_index)
    {
        if (parabola_index - 2 < 0 || parabola_index + 2 > this->beachline.size() - 1 ||
            this->beachline[parabola_index - 2]->ptr == this->beachline[parabola_index + 2]->ptr) return;

        double ix, iy, r;
        if (!checkCircle_edge(parabola_index, &ix, &iy, &r)) return;
        // if (!checkCircle_circle(parabola_index, &ix, &iy, &r)) return;

        // insertEvent(Event{Event::CIRCLE, ix, iy + r, this->beachline[parabola_index].get()});
        this->events.insert(Event{Event::CIRCLE, ix, iy + r, this->beachline[parabola_index].get()});
    }

    // void insertEvent(Event event)
    // {
    //     if (this->events.empty() || event.y > this->events.back().y) {
    //         this->events.push_back(event);
    //         return;
    //     }

    //     int index = 0;
    //     while (index < this->events.size() - 1) {
    //         if (event.y < this->events[index].y) break;
    //         index++;
    //     }

    //     this->events.insert(this->events.begin() + index, event);
    // }



    bool checkCircle_edge(int parabola_index, double* ix, double* iy, double* r)
    {
        HalfEdge* edge_left = static_cast<HalfEdge*>(this->beachline[parabola_index - 1]->ptr);
        Site* parabola = static_cast<Site*>(this->beachline[parabola_index]->ptr);
        HalfEdge* edge_right = static_cast<HalfEdge*>(this->beachline[parabola_index + 1]->ptr);

        if (!intersection_ray(
                edge_left->start.x, edge_left->start.y, edge_left->dir.x, edge_left->dir.y,
                edge_right->start.x, edge_right->start.y, edge_right->dir.x, edge_right->dir.y,
                ix, iy
            )
        ) return false;

        double dx = parabola->point.x - *ix;
        double dy = parabola->point.y - *iy;
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



    // bool checkCircle_circle(int parabola_index, double* ix, double* iy, double* r)
    // {

    // }



    bool clipHalfEdge(HalfEdge* halfEdge, double ix, double iy)
    {
        if (halfEdge->start.x >= 0 && halfEdge->start.x < this->width && halfEdge->start.y >= 0 && halfEdge->start.y < this->height) {
            double clipped_x = ix;
            double clipped_y = iy;
            if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) {
                clipRay(halfEdge->start.x, halfEdge->start.y, ix - halfEdge->start.x, iy - halfEdge->start.y, &clipped_x, &clipped_y);
            }
            halfEdge->dir.x = clipped_x - halfEdge->start.x;
            halfEdge->dir.y = clipped_y - halfEdge->start.y;
            return true;
        }

        if (ix >= 0 && ix < this->width && iy >= 0 && iy < this->height) {
            double clipped_x = halfEdge->start.x;
            double clipped_y = halfEdge->start.y;
            if (halfEdge->start.x < 0 || halfEdge->start.x >= this->width || halfEdge->start.y < 0 || halfEdge->start.y >= this->height) {
                clipRay(ix, iy, halfEdge->start.x - ix, halfEdge->start.y - iy, &clipped_x, &clipped_y);
            }
            halfEdge->start.x = ix;
            halfEdge->start.y = iy;
            halfEdge->dir.x = clipped_x - halfEdge->start.x;
            halfEdge->dir.y = clipped_y - halfEdge->start.y;
            return true;
        }

        return false;
    }

    void clipRay(double x1, double y1, double dir_x, double dir_y, double* ix, double* iy)
    {
        double tMin = INFINITY;

        if (dir_x != 0) {
            double x = (dir_x < 0 ? 0 : this->width - 1);
            double t = (x - x1) / dir_x;
            if (t > 0) {
                double y = y1 + t * dir_y;
                if (y >= 0 && y < this->height - 1) {
                    tMin = t;
                    *ix = x;
                    *iy = y;
                }
            }
        }

        if (dir_y != 0) {
            double y = (dir_y < 0 ? 0 : this->height - 1);
            double t = (y - y1) / dir_y;
            if (t > 0 && t < tMin) {
                double x = x1 + t * dir_x;
                if (x >= 0 && x < this->width - 1) {
                    tMin = t;
                    *ix = x;
                    *iy = y;
                }
            }
        }
    }



    void finishingHalfEdges()
    {
        for (int i = 0; i < this->beachline.size(); i++) {
            if (this->beachline[i]->type != Beachline::HALF_EDGE) continue;

            Site* site_left = static_cast<Site*>(this->beachline[i - 1]->ptr);
            HalfEdge* halfEdge = static_cast<HalfEdge*>(this->beachline[i]->ptr);
            Site* site_right = static_cast<Site*>(this->beachline[i + 1]->ptr);

            if ((halfEdge->start.x < 0 && halfEdge->start.x + halfEdge->dir.x < 0) ||
                (halfEdge->start.y < 0 && halfEdge->start.y + halfEdge->dir.y < 0) ||
                (halfEdge->start.x >= this->width && halfEdge->start.x + halfEdge->dir.x >= this->width) ||
                (halfEdge->start.y >= this->height && halfEdge->start.y + halfEdge->dir.y >= this->height)
            ) continue;

            double end_x, end_y;
            clipRay(halfEdge->start.x, halfEdge->start.y, halfEdge->dir.x, halfEdge->dir.y, &end_x, &end_y);

            halfEdge->dir.x = end_x - halfEdge->start.x;
            halfEdge->dir.y = end_y - halfEdge->start.y;

            site_left->addEdge(halfEdge);
            site_right->addEdge(halfEdge);
        }
    }
};
