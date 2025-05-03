#pragma once

#include "canvas.h"
#include "color.h"

#include "line.h"

struct Circle
{
    static void draw(Canvas* canvas, int cx, int cy, int r, Color color)
    {
        int x = 0;
        int y = -r;
        int p = 2 - 2 * r;

        while (y < 0) {
            canvas->setPixel(cx - x, cy + y, color);
            canvas->setPixel(cx - y, cy - x, color);
            canvas->setPixel(cx + x, cy - y, color);
            canvas->setPixel(cx + y, cy + x, color);
            r = p;
            if (r <= x) p += ++x * 2 + 1;
            if (r > y || p > x) p += ++y * 2 + 1;
        }
    }

    static void draw_midpoint(Canvas* canvas, int cx, int cy, int r, Color color)
    {
        int x = 0;
        int y = -r;
        int p = -r;

        while (x < -y) {
            if (p > 0) p += 2 * (x + ++y) + 1;
            else p += 2 * x + 1;
            canvas->setPixel(cx + x, cy + y, color);
            canvas->setPixel(cx - x, cy + y, color);
            canvas->setPixel(cx + x, cy - y, color);
            canvas->setPixel(cx - x, cy - y, color);
            canvas->setPixel(cx + y, cy + x, color);
            canvas->setPixel(cx - y, cy + x, color);
            canvas->setPixel(cx + y, cy - x, color);
            canvas->setPixel(cx - y, cy - x, color);
            x++;
        }
    }

    static void draw_filled(Canvas* canvas, int cx, int cy, int r, Color color)
    {
        int rr = r;
        int x = -r;
        int y = 0;
        int p = 2 - 2 * r;

        for (int i = cx + x; i <= cx - x; i++) canvas->setPixel(i, cy + y, color);
        while (y < rr) {
            r = p;
            if (r > x || p > y) p += ++x * 2 + 1;
            if (r > y) continue;
            p += ++y * 2 + 1;
            for (int i = cx + x; i <= cx - x; i++) {
                canvas->setPixel(i, cy + y, color);
                canvas->setPixel(i, cy - y, color);
            }
        }
    }



    /*
        A = x1 * (y2 − y3) − y1 * (x2 − x3) + x2 * y3 − x3 * y2
        B = (x1*x1 + y1*y1) * (y3 − y2) + (x2*x2 + y2*y2) * (y1 − y3) + (x3*x3 + y3*y3) * (y2 − y1)
        C = (x1*x1 + y1*y1) * (x2 − x3) + (x2*x2 + y2*y2) * (x3 − x1) + (x3*x3 + y3*y3) * (x1 − x2)
        D = (x1*x1 + y1*y1) * (x3*y2 − x2*y3) + (x2*x2 + y2*y2) * (x1*y3 − x3*y1) + (x3*x3 + y3*y3) * (x2*y1 − x1*y2)

        If A=0 then the points are colinear elasewhere using A,B,C you can find center and radius of circle

        xc = -B / 2 * A
        yc = -C / 2 * A
        r  = sqrt((B*B + C*C - 4 * A * D) / (4 * (A * A)))
    */

    // bool Voronoi::getCircle(float x1, float y1, float x2, float y2, float x3, float y3, float* cx, float* cy, float* r)
    // {
    //     float A = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    //     float B = (x1*x1 + y1*y1) * (y3 - y2) + (x2*x2 + y2*y2) * (y1 - y3) + (x3*x3 + y3*y3) * (y2 - y1);
    //     float C = (x1*x1 + y1*y1) * (x2 - x3) + (x2*x2 + y2*y2) * (x3 - x1) + (x3*x3 + y3*y3) * (x1 - x2);
    //     float D = (x1*x1 + y1*y1) * (x3*y2 - x2*y3) + (x2*x2 + y2*y2) * (x1*y3 - x3*y1) + (x3*x3 + y3*y3) * (x2*y1 - x1*y2);

    //     // If A=0 then the points are colinear elasewhere using A,B,C you can find center and radius of circle
    //     if (A == 0) return false;

    //     *cx = -B / 2 * A;
    //     *cy = -C / 2 * A;
    //     *r  = sqrt((B*B + C*C - 4 * A * D) / (4 * (A * A)));

    //     return true;
    // }

    static bool getCircle(float x1, float y1, float x2, float y2, float x3, float y3, float* cx, float* cy, float* r)
    {
        if ((x1 == x2 && y1 == y2) || (x2 == x3 && y2 == y3) || (x3 == x1 && y3 == y1)) return false;

        float x = x1;
        float y = y1;
        float x4 = x1;
        float y4 = y1;

        Line::setPerpendicular(&x1, &y1, &x2, &y2);
        Line::setPerpendicular(&x3, &y3, &x4, &y4);
        if (!Line::intersection_line(x1, y1, x2, y2, x3, y3, x4, y4, cx, cy)) {
            return false;
        }

        float dx = *cx - x;
        float dy = *cy - y;
        *r = sqrt(dx * dx + dy * dy);

        return true;
    }
};
