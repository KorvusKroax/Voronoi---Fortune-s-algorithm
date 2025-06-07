#pragma once

struct Edge
{
    double x1, y1, x2, y2;

    Edge() : x1(0), y1(0), x2(0), y2(0) { }

    Edge(double x1, double y1, double x2, double y2) : x1(x1), y1(y1), x2(x2), y2(y2) { }

    virtual ~Edge() = default;
};

struct HalfEdge : public Edge
{
    double x, dir_x;
    double y, dir_y;
    HalfEdge* otherHalf;

    HalfEdge(double x, double y, double dir_x, double dir_y, HalfEdge* otherHalf) : x(x), y(y), dir_x(dir_x), dir_y(dir_y), otherHalf(otherHalf) { }
};
