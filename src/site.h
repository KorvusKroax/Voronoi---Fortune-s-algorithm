#pragma once

#include <vector>

#include "edge.h"

struct Site {
    float x, y;
    std::vector<Edge> edges;

    Site() { }

    Site(float x, float y)
    {
        this->x = x;
        this->y = y;

        this->edges.clear();
    }

    void addEdge(float x1, float y1, float x2, float y2)
    {
        Edge edge(x1, y1, x2, y2);
        if (cross(x1, y1, x2, y2, x, y) > 0) edge = Edge(x2, y2, x1, y1);

        this->edges.push_back(edge);
    }

    inline float cross(float x1, float y1, float x2, float y2, float px, float py)
    {
        return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
        // result > 0: p is left
        // result = 0: p is on line
        // result < 0: p is right
    }
};
