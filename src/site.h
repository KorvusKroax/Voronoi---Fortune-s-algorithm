#pragma once

#include <vector>

#include "edge.h"

struct Site
{
    double x, y;
    std::vector<Edge*> edges;

    Site() : x(0), y(0) { }

    Site(double x, double y) : x(x), y(y) { }

    void addEdge(HalfEdge* halfEdge)
    {
        int otherIndex = -1;
        for (int i = 0; i < this->edges.size(); i++) {
            if (edges[i] == halfEdge->otherHalf) {
                otherIndex = i;
                break;
            };
        }

        if (otherIndex == -1) {
            edges.push_back(halfEdge);
            return;
        }

        this->edges.erase(this->edges.begin() + otherIndex);

        int x1 = halfEdge->otherHalf->x + halfEdge->otherHalf->dir_x;
        int y1 = halfEdge->otherHalf->y + halfEdge->otherHalf->dir_y;
        int x2 = halfEdge->x + halfEdge->dir_x;
        int y2 = halfEdge->y + halfEdge->dir_y;

        Edge* finishedEdge = new Edge(x1, y1, x2, y2);
        edges.push_back(finishedEdge);
    }

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
