#pragma once

#include <deque>
#include <vector>
#include <algorithm> // for std::sort
#include <cmath> // for std::sqrt
#include <memory> // for std::unique_ptr
#include <iostream> // for std::cout and std::endl

// #include "edge.h"
#include "half_edge.h"
#include "site.h"
#include "event.h"
#include "beachline.h"

class Fortune
{
    public:
        double width;
        double height;
        int siteCount;
        Site* sites;

        std::deque<Event> events;
        std::vector<std::unique_ptr<Beachline>> beachline;

        std::vector<HalfEdge*> finishedEdges;

        Fortune() { }

        void create(double width, double height, int siteCount, Site* sites)
        {
            this->width = width;
            this->height = height;
            this->siteCount = siteCount;
            this->sites = sites;

            init();

            this->beachline.push_back(
                std::make_unique<Beachline>(
                    Beachline(PARABOLA, static_cast<Site*>(events.front().ptr))
                )
            );
            this->events.pop_front();

            while (!this->events.empty()) {
                Event event = this->events.front();

                if (event.type == SITE) {
                    handle_siteEvent(&event);
                } else if (event.type == CIRCLE) {
                    handle_circleEvent(&event);
                }

                this->events.pop_front();
            }

            finishingHalfEdges();
        }

        void init()
        {
            this->events.clear();

            for (int i = 0; i < siteCount; i++) {
                this->events.emplace_back(SITE, sites[i].x, sites[i].y, &sites[i]);
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
                std::make_unique<Beachline>(PARABOLA, parabola_below)
            );

            double dx = parabola_below->x - new_site->x;
            double dy = parabola_below->y - new_site->y;
            double edge_origin_y = ((dx * dx) / (dy * 2.0)) + ((parabola_below->y + new_site->y) / 2.0);

            HalfEdge* edge_left = new HalfEdge(new_site->x, edge_origin_y, dy, -dx, nullptr);
            HalfEdge* edge_right = new HalfEdge(new_site->x, edge_origin_y, -dy, dx, nullptr);
            edge_left->otherHalf = edge_right;
            edge_right->otherHalf = edge_left;

            this->beachline.insert(this->beachline.begin() + parabola_below_index + 1, std::make_unique<Beachline>(HALF_EDGE, edge_left));
            this->beachline.insert(this->beachline.begin() + parabola_below_index + 2, std::make_unique<Beachline>(PARABOLA, new_site));
            this->beachline.insert(this->beachline.begin() + parabola_below_index + 3, std::make_unique<Beachline>(HALF_EDGE, edge_right));

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

            if (edge_left->x >= 0 && edge_left->x < this->width && edge_left->y >= 0 && edge_left->y < this->height) {
                double clipped_x = ix;
                double clipped_y = iy;
                if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) {
                    clipEdge(edge_left->x, edge_left->y, ix - edge_left->x, iy - edge_left->y, &clipped_x, &clipped_y);
                }
                edge_left->dir_x = clipped_x - edge_left->x;
                edge_left->dir_y = clipped_y - edge_left->y;
                parabola_left->edges.push_back(edge_left);
                parabola->edges.push_back(edge_left);
            } else if (ix >= 0 && ix < this->width && iy >= 0 && iy < this->height) {
                double clipped_x = edge_left->x;
                double clipped_y = edge_left->y;
                if (edge_left->x < 0 || edge_left->x >= this->width || edge_left->y < 0 || edge_left->y >= this->height) {
                    clipEdge(ix, iy, edge_left->x - ix, edge_left->y - iy, &clipped_x, &clipped_y);
                }
                edge_left->x = ix;
                edge_left->y = iy;
                edge_left->dir_x = clipped_x - edge_left->x;
                edge_left->dir_y = clipped_y - edge_left->y;
                parabola_left->edges.push_back(edge_left);
                parabola->edges.push_back(edge_left);
            }

            if (edge_right->x >= 0 && edge_right->x < this->width && edge_right->y >= 0 && edge_right->y < this->height) {
                double clipped_x = ix;
                double clipped_y = iy;
                if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) {
                    clipEdge(edge_right->x, edge_right->y, ix - edge_right->x, iy - edge_right->y, &clipped_x, &clipped_y);
                }
                edge_right->dir_x = clipped_x - edge_right->x;
                edge_right->dir_y = clipped_y - edge_right->y;
                parabola_right->edges.push_back(edge_right);
                parabola->edges.push_back(edge_right);
            } else if (ix >= 0 && ix < this->width && iy >= 0 && iy < this->height) {
                double clipped_x = edge_right->x;
                double clipped_y = edge_right->y;
                if (edge_right->x < 0 || edge_right->x >= this->width || edge_right->y < 0 || edge_right->y >= this->height) {
                    clipEdge(ix, iy, edge_right->x - ix, edge_right->y - iy, &clipped_x, &clipped_y);
                }
                edge_right->x = ix;
                edge_right->y = iy;
                edge_right->dir_x = clipped_x - edge_right->x;
                edge_right->dir_y = clipped_y - edge_right->y;
                parabola_right->edges.push_back(edge_right);
                parabola->edges.push_back(edge_right);
            }

            parabola_index--;
            this->beachline.erase(this->beachline.begin() + parabola_index); // remove left edge
            this->beachline.erase(this->beachline.begin() + parabola_index); // remove parabola
            this->beachline.erase(this->beachline.begin() + parabola_index); // remove right edge

            float dx = parabola_right->x - parabola_left->x;
            float dy = parabola_right->y - parabola_left->y;
            HalfEdge* new_edge = new HalfEdge(ix, iy, -dy, dx, nullptr);
            this->beachline.insert(this->beachline.begin() + parabola_index,
                std::make_unique<Beachline>(HALF_EDGE, new_edge)
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
            for (int i = 0; i < this->events.size(); i++) {
                if (this->events[i].type == CIRCLE && this->events[i].ptr == this->beachline[parabola_index].get()) {
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

            insertEvent(Event(CIRCLE, ix, iy + r, this->beachline[parabola_index].get()));
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

        void finishingHalfEdges()
        {
            finishedEdges.clear();

            for (int i = 0; i < this->beachline.size(); i++) {
                if (this->beachline[i]->type != HALF_EDGE) continue;

                Site* site_left = static_cast<Site*>(this->beachline[i - 1]->ptr);
                HalfEdge* halfEdge = static_cast<HalfEdge*>(this->beachline[i]->ptr);
                Site* site_right = static_cast<Site*>(this->beachline[i + 1]->ptr);

                if ((halfEdge->x < 0 && halfEdge->x + halfEdge->dir_x < 0) ||
                    (halfEdge->y < 0 && halfEdge->y + halfEdge->dir_y < 0) ||
                    (halfEdge->x >= this->width && halfEdge->x + halfEdge->dir_x >= this->width) ||
                    (halfEdge->y >= this->height && halfEdge->y + halfEdge->dir_y >= this->height)
                ) continue;

                double end_x, end_y;
                clipEdge(halfEdge->x, halfEdge->y, halfEdge->dir_x, halfEdge->dir_y, &end_x, &end_y);

                halfEdge->dir_x = end_x - halfEdge->x;
                halfEdge->dir_y = end_y - halfEdge->y;

                site_left->edges.push_back(halfEdge);
                site_right->edges.push_back(halfEdge);

                finishedEdges.push_back(halfEdge);
            }
        }

        void clipEdge(double x1, double y1, double dir_x, double dir_y, double* ix, double* iy)
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
};
