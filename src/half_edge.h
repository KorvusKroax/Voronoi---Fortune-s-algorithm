#pragma once

struct HalfEdge {
    float x, y, dir_x, dir_y;

    HalfEdge(float x, float y, float dir_x, float dir_y)
    {
        this->x = x;
        this->y = y;
        this->dir_x = dir_x;
        this->dir_y = dir_y;
    }
};
