#pragma once

#include <cmath>

// EGA colors
#define EGA_BLACK           Color(0x00, 0x00, 0x00)
#define EGA_BLUE            Color(0x00, 0x00, 0xA8)
#define EGA_GREEN           Color(0x00, 0xA8, 0x00)
#define EGA_CYAN            Color(0x00, 0xA8, 0xA8)
#define EGA_RED             Color(0xA8, 0x00, 0x00)
#define EGA_MAGENTA         Color(0xA8, 0x00, 0xA8)
#define EGA_ORANGE          Color(0xA8, 0x54, 0x00)
#define EGA_GREY            Color(0xA8, 0xA8, 0xA8)
#define EGA_DARK_GREY       Color(0x54, 0x54, 0x54)
#define EGA_LIGHT_BLUE      Color(0x54, 0x54, 0xFE)
#define EGA_LIGHT_GREEN     Color(0x54, 0xFE, 0x54)
#define EGA_LIGHT_CYAN      Color(0x54, 0xFE, 0xFE)
#define EGA_BRIGHT_RED      Color(0xFE, 0x54, 0x54)
#define EGA_BRIGHT_MAGENTA  Color(0xFE, 0x54, 0xFE)
#define EGA_YELLOW          Color(0xFE, 0xFE, 0x54)
#define EGA_WHITE           Color(0xFE, 0xFE, 0xFE)

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
