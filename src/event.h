#pragma once

struct Event
{
    enum Type { SITE, CIRCLE };

    Type type;
    double x;
    double y;
    void* ptr;
        // SITE: Site*
        // CIRCLE: Beachline*

    Event(Type type, double x, double y, void* ptr) : type(type), x(x), y(y), ptr(ptr) { }
};
