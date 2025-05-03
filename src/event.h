#pragma once

enum EventType { SITE, CIRCLE };

struct Event
{
    EventType type;

    float y;
    int index;
        // for site event -> site index
        // for circle event -> beachline parabola index

    Event(EventType type, float y, int index)
    {
        this->type = type;

        this->y = y;
        this->index = index;
    }
};
