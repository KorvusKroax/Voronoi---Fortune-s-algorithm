#pragma once

#include <cmath>

#include "palette_ega.h"
#include "palette_c64.h"

struct Color {
    unsigned int value;

    Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255)
    {
        this->value = r | (g << 8) | (b << 16) | (a << 24);
    }

    Color(unsigned int value = 0, unsigned int a = 255)
    {
        this->value = (value & 0x00ffffff) | (a << 24);
    }

    inline int getRed() { return this->value & 0x000000ff; }
    inline int getGreen() { return (this->value >> 8) & 0x000000ff; }
    inline int getBlue() { return (this->value >> 16) & 0x000000ff; }
    inline int getAlpha() { return (this->value >> 24) & 0x000000ff; }

    void setRed(unsigned int v) { this->value = this->value & 0xffffff00 | v; }
    void setGreen(unsigned int v) { this->value = this->value & 0xffff00ff | (v << 8); }
    void setBlue(unsigned int v) { this->value = this->value & 0xff00ffff | (v << 16); }
    void setAlpha(unsigned int v) { this->value = this->value & 0x00ffffff | (v << 24); }

    int grayscale()
    {
        int v = getRed() * 0.299f + getGreen() * 0.587f + getBlue() * 0.114f;
        return v | (v << 8) | (v << 16) | (getAlpha() << 24);
    }
};
