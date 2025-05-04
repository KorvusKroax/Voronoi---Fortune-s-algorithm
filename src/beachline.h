#pragma once

enum BeachLineType { PARABOLA, EDGE };

struct BeachLine
{
    int id;
    BeachLineType type;
    void* ptr;
        // for parabola -> Site*
        // for edge -> Edge*

    BeachLine(int id, BeachLineType type, void* ptr)
    {
        this->id = id;
        this->type = type;
        this->ptr = ptr;
    }
};
