#pragma once

enum BeachlineType { PARABOLA, HALF_EDGE };

struct Beachline
{
    BeachlineType type;
    void* ptr;
        // PARABOLA: Site*
        // HALF_EDGE: HalfEdge*

    Beachline(BeachlineType type, void* ptr) : type(type), ptr(ptr) { }
};
