#pragma once

enum BeachlineType { PARABOLA, EDGE };

struct Beachline
{
    int id;
    BeachlineType type;
    void* ptr;
        // for parabola -> Site*
        // for edge -> Edge*

    Beachline(int id, BeachlineType type, void* ptr)
    {
        this->id = id;
        this->type = type;
        this->ptr = ptr;
    }
};
