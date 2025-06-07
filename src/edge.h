#pragma once

struct Edge {
    double x, y, x2, y2;

    Edge(double x1, double y1, double x2, double y2) : x(x1), y(y1), x2(x2), y2(y2) { }

    virtual ~Edge() = default;
};

struct HalfEdge : public Edge
{
    double dir_x;
    double dir_y;
    HalfEdge* otherHalf;

    HalfEdge(double x, double y, double dir_x, double dir_y, HalfEdge* otherHalf) : Edge(x, y, x, y), dir_x(dir_x), dir_y(dir_y), otherHalf(otherHalf) { }
};
