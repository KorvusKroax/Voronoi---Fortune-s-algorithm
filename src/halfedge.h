#pragma once

struct HalfEdge {
    float x, y, slope;

    HalfEdge() { }

    HalfEdge(float x, float y, float slope)
    {
        this->x = x;
        this->y = y;
        this->slope = slope;
    }
};
