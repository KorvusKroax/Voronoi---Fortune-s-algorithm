#pragma once

#include "beachline_parabola.h"

struct Event
{
    float y;
};

struct SiteEvent : public Event
{
    BeachlineParabola* parabola;

    SiteEvent() { }

    SiteEvent(float y, BeachlineParabola* parabola)
    {
        this->y = y;
        this->parabola = parabola;
    }
};

struct CircleEvent : public Event
{
    float x, r;

    CircleEvent() { }

    CircleEvent(float x, float y, float r)
    {
        this->x = x;
        this->y = y;
        this->r = r;
    }
};
