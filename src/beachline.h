#pragma once

enum BeachLineType { PARABOLA, EDGE };

struct BeachLine
{
    BeachLineType type;
    void* ptr;
        // for parabola -> site
        // for edge -> edge

    BeachLine(BeachLineType type, void* ptr)
    {
        this->type = type;
        this->ptr = ptr;
    }
};
