#pragma once

struct Beachline
{
    enum Type { PARABOLA, HALF_EDGE };

    Type type;
    void* ptr;
        // PARABOLA: Site*
        // HALF_EDGE: HalfEdge*

    Beachline(Type type, void* ptr) : type(type), ptr(ptr) { }
};
