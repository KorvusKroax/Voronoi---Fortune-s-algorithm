#pragma once

enum EventType { SITE, CIRCLE };

struct Event
{
    EventType type;
    double x;
    double y;
    void* ptr;
        // SITE: Site*
        // CIRCLE: Beachline*

    Event(EventType type, double x, double y, void* ptr) : type(type), x(x), y(y), ptr(ptr) { }
};
