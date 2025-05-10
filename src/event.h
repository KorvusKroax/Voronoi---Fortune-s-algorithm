#pragma once

enum EventType { SITE, CIRCLE };

struct Event
{
    EventType type;

    float y;
    void* ptr;
        // for site event -> Site*
        // for circle event -> Beachline* (parabola)

    Event(EventType type, float y, void* ptr)
    {
        this->type = type;

        this->y = y;
        this->ptr = ptr;
    }
};
