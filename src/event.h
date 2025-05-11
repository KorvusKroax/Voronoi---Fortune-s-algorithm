#pragma once

enum EventType { SITE, CIRCLE };

struct Event
{
    EventType type;

    float x, y;
    void* ptr;
        // for SITE -> Site*
        // for CIRCLE -> Beachline* (parabola)

    Event(EventType type, float x, float y, void* ptr)
    {
        this->type = type;
        this->x = x;
        this->y = y;
        this->ptr = ptr;
    }

    bool operator < (const Event& other) const
    {
        return this->y < other.y || (this->y == other.y && this->x < other.x);
    }
};
