#pragma once

#include "site.h"

namespace BeachLine
{
    enum Type { PARABOLA, EDGE };

    struct Base
    {
        virtual Type getType() = 0;
    };

    struct Parabola : public Base
    {
        Site* site;

        Parabola(Site* site)
        {
            this->site = site;
        }

        Type getType() { return PARABOLA; }
    };

    struct Edge : public Base
    {
        float x, y, dx, dy;
        float slope;

        Edge(float x, float y, float dx, float dy)
        {
            this->x = x;
            this->y = y;
            this->dx = dx;
            this->dy = dy;

            this->slope = this->dx / this->dy;
        }

        Type getType() { return EDGE; }
    };
}
