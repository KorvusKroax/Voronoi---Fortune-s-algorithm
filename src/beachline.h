#pragma once

enum BeachLineType { PARABOLA, EDGE };

struct BeachLine
{
    int id;
    BeachLineType type;
    void* ptr;
        // for parabola -> Site*
        // for edge -> Edge*

    BeachLine(BeachLineType type, void* ptr)
    {
        this->id = unique_id++;
        this->type = type;
        this->ptr = ptr;
    }

    private: static int unique_id;
};
