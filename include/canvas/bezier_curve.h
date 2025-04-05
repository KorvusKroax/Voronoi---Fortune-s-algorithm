#pragma once

#include "canvas.h"
#include "color.h"
#include "line.h"
#include "circle.h"

struct BezierCurve
{
    static void draw_quadratic(Canvas* canvas, int x1, int y1, int xc, int yc, int x2, int y2, float resolution, Color color)
    {
        float xA, xB, xp, x = x1;
        float yA, yB, yp, y = y1;
        float step = 1.0f / resolution;

        for (float t = step; t < 1 + step; t += step) {
            xp = x;
            yp = y;

            xA = x1 + (xc - x1) * t;
            yA = y1 + (yc - y1) * t;
            xB = xc + (x2 - xc) * t;
            yB = yc + (y2 - yc) * t;

            x = xA + (xB - xA) * t;
            y = yA + (yB - yA) * t;
            Line::draw(canvas, (int)xp, (int)yp, (int)x, (int)y, color);
        }
        Line::draw(canvas, (int)x, (int)y, (int)x2, (int)y2, color);
    }

    static void draw_cubic(Canvas* canvas, int x1, int y1, int x1c, int y1c, int x2c, int y2c, int x2, int y2, float resolution, Color color)
    {
        float xA, yA, xB, yB, xC, yC;
        float xAB, yAB, xBC, yBC;

        float xp, x = x1;
        float yp, y = y1;
        float step = 1.0f / resolution;
        for (float t = step; t < 1 + step; t += step) {
            xp = x;
            yp = y;

            xA = x1 + (x1c - x1) * t;
            yA = y1 + (y1c - y1) * t;
            xB = x1c + (x2c - x1c) * t;
            yB = y1c + (y2c - y1c) * t;
            xC = x2c + (x2 - x2c) * t;
            yC = y2c + (y2 - y2c) * t;

            xAB = xA + (xB - xA) * t;
            yAB = yA + (yB - yA) * t;
            xBC = xB + (xC - xB) * t;
            yBC = yB + (yC - yB) * t;

            x = xAB + (xBC - xAB) * t;
            y = yAB + (yBC - yAB) * t;
            Line::draw(canvas, xp, yp, x, y, color);
        }
        Line::draw(canvas, x, y, x2, y2, color);
    }
};
