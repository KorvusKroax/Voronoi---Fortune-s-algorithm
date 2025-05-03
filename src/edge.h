#pragma once

struct Edge {
    float x, y, dx, dy;

    Edge(float x, float y, float dx, float dy)
    {
        this->x = x;
        this->y = y;
        this->dx = dx;
        this->dy = dy;
    }
};
