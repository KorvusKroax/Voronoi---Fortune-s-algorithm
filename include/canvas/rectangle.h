#pragma once

#include "canvas.h"
#include "color.h"

struct Rectangle
{
    static void draw(Canvas* canvas, int x, int y, int width, int height, Color color)
    {
        for (int i = 0; i < width; i++) {
            canvas->setPixel(x + i, y, color);
            canvas->setPixel(x + i, y + height - 1, color);
        }
        for (int i = 1; i < height - 1; i++) {
            canvas->setPixel(x, y + i, color);
            canvas->setPixel(x + width - 1, y + i, color);
        }
    }

    static void draw_filled(Canvas* canvas, int x, int y, int width, int height, Color color)
    {
        for (int j = 0; j < height; j++)  {
            for (int i = 0; i < width; i++) {
                canvas->setPixel(x + i, y + j, color);
            }
        }
    }
};
