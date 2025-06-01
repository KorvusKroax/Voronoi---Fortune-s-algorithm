#pragma once

struct HalfEdge
{
    double x;
    double y;
    double dir_x;
    double dir_y;
    HalfEdge* otherHalf;

    HalfEdge(double x, double y, double dir_x, double dir_y, HalfEdge* otherHalf) : x(x), y(y), dir_x(dir_x), dir_y(dir_y), otherHalf(otherHalf) { }
};
