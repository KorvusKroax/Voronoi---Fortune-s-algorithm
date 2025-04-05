#pragma once

#include "color.h"

struct Canvas
{
    unsigned int width, height;
    int* pixels;

    Canvas();
    Canvas(unsigned int width, unsigned int height);
    Canvas(const char* fileName);
    ~Canvas();

    void init(unsigned int width, unsigned int height);

    void clearCanvas();
    void fillCanvas(Color color);

    void setPixel(int x, int y, Color color);
    bool getPixel(int x, int y, Color* color);

    void setPixels(int x, int y, Canvas* canvas);
    bool getPixels(int x, int y, unsigned int w, unsigned int h, Canvas* canvas);

    bool loadImage_PNG(const char* fileName);
};

// #include "bezier_curve.h"
// #include "circle.h"
// #include "fill.h"
// #include "line.h"
// #include "rectangle.h"
// #include "triangle.h"
// #include "text.h"
