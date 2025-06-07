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

        double x1 = halfEdge->otherHalf->x + halfEdge->otherHalf->dir_x;
        double y1 = halfEdge->otherHalf->y + halfEdge->otherHalf->dir_y;
        double x2 = halfEdge->x + halfEdge->dir_x;
        double y2 = halfEdge->y + halfEdge->dir_y;
        edges.push_back(
            cross(x1, y1, x2, y2, this->x, this->y) < 0 ?
                new Edge(x1, y1, x2, y2) :
                new Edge(x2, y2, x1, y1)
        );
    }

    void updateEdges()
    {
        halfEdgesToEdges();
    }

    void halfEdgesToEdges()
    {
        std::vector<Edge*> newEdges;

        for (Edge* edge : this->edges) {
            if (HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(edge)) {
                double x1 = halfEdge->x;
                double y1 = halfEdge->y;
                double x2 = halfEdge->x + halfEdge->dir_x;
                double y2 = halfEdge->y + halfEdge->dir_y;
                newEdges.push_back(
                    cross(x1, y1, x2, y2, this->x, this->y) < 0 ?
                        new Edge(x1, y1, x2, y2) :
                        new Edge(x2, y2, x1, y1)
                );
            } else {
                newEdges.push_back(edge);
            }
        }

        this->edges = std::move(newEdges);
    }

    inline double cross(double x1, double y1, double x2, double y2, double px, double py)
    {
        return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
        // result > 0: p is left
        // result = 0: p is on line
        // result < 0: p is right
    }
};
