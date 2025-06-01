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
        for (int j = 1; j < height - 1; j++) {
            canvas->setPixel(x, y + j, color);
            canvas->setPixel(x + width - 1, y + j, color);
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
