#pragma once

enum BeachlineType { PARABOLA, EDGE };

struct Beachline
{
    int id;
    BeachlineType type;
    void* ptr;
        // for PARABOLA -> Site*
        // for EDGE -> HalfEdge*

    Beachline(int id, BeachlineType type, void* ptr)
    {
        this->id = id;
        this->type = type;
        this->ptr = ptr;
    }
};
