#pragma once

#include <set>
#include <vector>
#include <algorithm> // for std::find_if
#include <cmath> // for std::sqrt
#include <memory> // for std::unique_ptr
#include <iostream> // for std::cout and std::endl
#include <variant> // for std::holds_alternative

#include "edge.h"
#include "site.h"



enum EventType { SITE, CIRCLE };
struct Event
{
    EventType type;
    double x, y;
    void* ptr; // SITE: Site*, CIRCLE: Beachline*
    bool operator < (const Event& other) const { return this->y < other.y || (this->y == other.y && this->x < other.x); }
};



class Fortune
{
    public:
        double width, height;
        size_t siteCount;
        Site* sites;

        std::set<Event> events;
        std::vector<std::unique_ptr<std::variant<Site*, HalfEdge*>>> beachline;
        std::vector<HalfEdge*> halfEdges;

        Fortune(double width, double height, std::vector<std::pair<double, double>>* points)
        {
            this->width = width;
            this->height = height;

            // fill up sites with points
            this->siteCount = points->size();
            this->sites = new Site[this->siteCount];
            for (size_t i = 0; i < this->siteCount; i++) {
                sites[i] = Site{(*points)[i].first, (*points)[i].second};
            }

            // fill up events initial values
            this->events.clear();
            for (size_t i = 0; i < this->siteCount; i++) {
                this->events.insert(Event{SITE, sites[i].x, sites[i].y, &sites[i]});
            }

            // make first event as initial beachline
            this->beachline.clear();
            Site* firstSite = static_cast<Site*>(events.begin()->ptr);
            this->beachline.push_back(std::make_unique<std::variant<Site*, HalfEdge*>>(firstSite));
            this->events.erase(this->events.begin());

            // processing other events
            while (!this->events.empty()) {
                Event event = *this->events.begin();
                if (event.type == SITE) {
                    handle_siteEvent(&event);
                } else if (event.type == CIRCLE) {
                    handle_circleEvent(&event);
                }
                this->events.erase(this->events.begin());
            }

            finishingHalfEdges();

            for (size_t i = 0; i < this->siteCount; i++) {
                this->sites[i].halfEdgesToEdges();
            }

            // free up memory
            for (HalfEdge* edge : this->halfEdges) delete edge;
            this->halfEdges.clear();
            this->beachline.clear();
            this->events.clear();
        }

        void handle_siteEvent(Event* event)
        {
            Site* new_site = static_cast<Site*>(event->ptr);

            size_t parabola_below_index = getParabolaIndexBelow(new_site);
            Site* parabola_below = std::get<Site*>(*this->beachline[parabola_below_index]);

            checkCircleEvent_remove(parabola_below_index);

            this->beachline.emplace(this->beachline.begin() + parabola_below_index, std::make_unique<std::variant<Site*, HalfEdge*>>(parabola_below));

            double dx = parabola_below->x - new_site->x;
            double dy = parabola_below->y - new_site->y;
            double edge_start_y = ((dx * dx) / (dy * 2.0)) + ((parabola_below->y + new_site->y) / 2.0);

            HalfEdge* edge_left = new HalfEdge(new_site->x, edge_start_y, dy, -dx, nullptr);
            HalfEdge* edge_right = new HalfEdge(new_site->x, edge_start_y, -dy, dx, nullptr);
            edge_left->otherHalf = edge_right;
            edge_right->otherHalf = edge_left;

            this->halfEdges.push_back(edge_left);
            this->halfEdges.push_back(edge_right);

            this->beachline.emplace(this->beachline.begin() + parabola_below_index + 1, std::make_unique<std::variant<Site*, HalfEdge*>>(edge_left));
            this->beachline.emplace(this->beachline.begin() + parabola_below_index + 2, std::make_unique<std::variant<Site*, HalfEdge*>>(new_site));
            this->beachline.emplace(this->beachline.begin() + parabola_below_index + 3, std::make_unique<std::variant<Site*, HalfEdge*>>(edge_right));

            checkCircleEvent_add(parabola_below_index);
            checkCircleEvent_add(parabola_below_index + 4);
        }

        void handle_circleEvent(Event* event)
        {
            size_t parabola_index = -1;
            for (size_t i = 0; i < this->beachline.size(); i += 2) {
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

            Site* parabola_left = std::get<Site*>(*this->beachline[parabola_index - 2]);
            Site* parabola = std::get<Site*>(*this->beachline[parabola_index]);
            Site* parabola_right = std::get<Site*>(*this->beachline[parabola_index + 2]);

            HalfEdge* edge_left = std::get<HalfEdge*>(*this->beachline[parabola_index - 1]);
            HalfEdge* edge_right = std::get<HalfEdge*>(*this->beachline[parabola_index + 1]);

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

            float dx = parabola_right->x - parabola_left->x;
            float dy = parabola_right->y - parabola_left->y;
            HalfEdge* new_edge = new HalfEdge(ix, iy, -dy, dx, nullptr);

            this->halfEdges.push_back(new_edge);

            this->beachline.emplace(this->beachline.begin() + parabola_index, std::make_unique<std::variant<Site*, HalfEdge*>>(new_edge));

            checkCircleEvent_add(parabola_index - 1);
            checkCircleEvent_add(parabola_index + 1);
        }



        int getParabolaIndexBelow(Site* site)
        {
            for (size_t i = 0; i < this->beachline.size() - 1; i += 2) {
                Site* parabola = std::get<Site*>(*this->beachline[i]);
                Site* parabola_next = std::get<Site*>(*this->beachline[i + 2]);
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
            std::variant<Site*, HalfEdge*>* ptr = this->beachline[parabola_index].get();
            std::set<Event>::iterator it = std::find_if(this->events.begin(), this->events.end(),
                [ptr](const Event& event) {return event.type == CIRCLE && event.ptr == ptr;}
            );

            if (it != this->events.end()) this->events.erase(it);
        }

        void checkCircleEvent_add(int parabola_index)
        {
            if (parabola_index - 2 < 0 || parabola_index + 2 > this->beachline.size() - 1 ||
                *this->beachline[parabola_index - 2] == *this->beachline[parabola_index + 2]) return;

            double ix, iy, r;
            if (!checkCircle_edge(parabola_index, &ix, &iy, &r)) return;

            // inserting new event into its sorted place
            this->events.insert(Event{CIRCLE, ix, iy + r, this->beachline[parabola_index].get()});
        }



        bool checkCircle_edge(int parabola_index, double* ix, double* iy, double* r)
        {
            HalfEdge* edge_left = std::get<HalfEdge*>(*this->beachline[parabola_index - 1]);
            Site* parabola = std::get<Site*>(*this->beachline[parabola_index]);
            HalfEdge* edge_right = std::get<HalfEdge*>(*this->beachline[parabola_index + 1]);

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



        // bool checkCircle_circle(int parabola_index, double* ix, double* iy, double* r)
        // {

        // }



        bool clipHalfEdge(HalfEdge* edge, double ix, double iy)
        {
            if (edge->x >= 0 && edge->x < this->width && edge->y >= 0 && edge->y < this->height) {
                double clipped_x = ix;
                double clipped_y = iy;
                if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) {
                    clipRay(edge->x, edge->y, ix - edge->x, iy - edge->y, &clipped_x, &clipped_y);
                }
                edge->dir_x = clipped_x - edge->x;
                edge->dir_y = clipped_y - edge->y;
                return true;
            }

            if (ix >= 0 && ix < this->width && iy >= 0 && iy < this->height) {
                double clipped_x = edge->x;
                double clipped_y = edge->y;
                if (edge->x < 0 || edge->x >= this->width || edge->y < 0 || edge->y >= this->height) {
                    clipRay(ix, iy, edge->x - ix, edge->y - iy, &clipped_x, &clipped_y);
                }
                edge->x = ix;
                edge->y = iy;
                edge->dir_x = clipped_x - edge->x;
                edge->dir_y = clipped_y - edge->y;
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
            for (size_t i = 0; i < this->beachline.size(); i++) {
                if (!std::holds_alternative<HalfEdge*>(*this->beachline[i])) continue;

                Site* site_left = std::get<Site*>(*this->beachline[i - 1]);
                HalfEdge* halfEdge = std::get<HalfEdge*>(*this->beachline[i]);
                Site* site_right = std::get<Site*>(*this->beachline[i + 1]);

                if ((halfEdge->x < 0 && halfEdge->x + halfEdge->dir_x < 0) ||
                    (halfEdge->y < 0 && halfEdge->y + halfEdge->dir_y < 0) ||
                    (halfEdge->x >= this->width && halfEdge->x + halfEdge->dir_x >= this->width) ||
                    (halfEdge->y >= this->height && halfEdge->y + halfEdge->dir_y >= this->height)
                ) continue;

                double end_x, end_y;
                clipRay(halfEdge->x, halfEdge->y, halfEdge->dir_x, halfEdge->dir_y, &end_x, &end_y);

                halfEdge->dir_x = end_x - halfEdge->x;
                halfEdge->dir_y = end_y - halfEdge->y;

                site_left->addEdge(halfEdge);
                site_right->addEdge(halfEdge);
            }
        }
};
