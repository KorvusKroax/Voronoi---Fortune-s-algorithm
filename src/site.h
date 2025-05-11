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
        // if (!checkUniqueEdge(x1, y1, x2, y2)) return;

        if (cross(x1, y1, x2, y2, x, y) < 0) {
            this->edges.push_back(Edge(x1, y1, x2, y2));
        } else {
            this->edges.push_back(Edge(x2, y2, x1, y1));
        }
    }

    // bool checkUniqueEdge(float x1, float y1, float x2, float y2)
    // {
    //     // float n = 1000.0f;
    //     // x1 = std::round(x1 * n) / n;
    //     // y1 = std::round(y1 * n) / n;
    //     // x2 = std::round(x2 * n) / n;
    //     // y2 = std::round(y2 * n) / n;

    //     float t = 0.001f;

    //     for (int i = 0; i < edges.size(); i++) {
    //         if (edges[i].x1 == x1 && edges[i].y1 == y1) {
    //             float c = cross(edges[i].x2, edges[i].y2, x2, y2, edges[i].x1, edges[i].y1);
    //             if (-t < c < t) {
    //                 edges[i].x1 = x2;
    //                 edges[i].y1 = y2;
    //                 return false;
    //             }
    //         }
    //         if (edges[i].x1 == x2 && edges[i].y1 == y2) {
    //             float c = cross(edges[i].x2, edges[i].y2, x1, y1, edges[i].x1, edges[i].y1);
    //             if (-t < c < t) {
    //                 edges[i].x1 = x1;
    //                 edges[i].y1 = y1;
    //                 return false;
    //             }
    //         }
    //         if (edges[i].x2 == x1 && edges[i].y2 == y1) {
    //             float c = cross(edges[i].x1, edges[i].y1, x2, y2, edges[i].x2, edges[i].y2);
    //             if (-t < c < t) {
    //                 edges[i].x2 = x2;
    //                 edges[i].y2 = y2;
    //                 return false;
    //             }
    //         }
    //         if (edges[i].x2 == x2 && edges[i].y2 == y2) {
    //             float c = cross(edges[i].x1, edges[i].y1, x1, y1, edges[i].x2, edges[i].y2);
    //             if (-t < c < t) {
    //                 edges[i].x2 = x1;
    //                 edges[i].y2 = y1;
    //                 return false;
    //             }
    //         }
    //     }
    //     return true;
    // }

    inline float cross(float x1, float y1, float x2, float y2, float px, float py)
    {
        return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
        // result > 0: p is left
        // result = 0: p is on line
        // result < 0: p is right
    }
};
