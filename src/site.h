#pragma once

#include <vector>

#include "half_edge.h"

struct Site
{
    double x, y;
    std::vector<HalfEdge*> edges;

    Site() : x(0), y(0) { this->edges.clear(); }

    Site(double x, double y) : x(x), y(y) { this->edges.clear(); }

    // void addEdge(double x1, double y1, double x2, double y2)
    // {
    //     this->edges.push_back(
    //         cross(x1, y1, x2, y2, this->x, this->y) < 0 ?
    //             Edge(x1, y1, x2, y2) :
    //             Edge(x2, y2, x1, y1)
    //     );
    // }

    // inline double cross(double x1, double y1, double x2, double y2, double px, double py)
    // {
    //     return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
    //     // result > 0: p is left
    //     // result = 0: p is on line
    //     // result < 0: p is right
    // }
};
