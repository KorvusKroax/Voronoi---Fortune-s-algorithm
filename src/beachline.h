#pragma once

enum BeachLineType { PARABOLA, EDGE };

struct BeachLine
{
    BeachLineType type;
    int index;
        // for parabola -> site index
        // for edge -> edge index

    BeachLine(BeachLineType type, int index)
    {
        this->type = type;
        this->index = index;
    }
};
