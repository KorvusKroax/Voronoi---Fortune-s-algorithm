#pragma once

#include <cmath> // abs, sqrt
#include "canvas.h"
#include "color.h"

struct Line
{
    static void draw(Canvas* canvas, int x1, int y1, int x2, int y2, Color color)
    {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;

        if (dx > dy) {
            int y = y1;
            int d = dy * 2 - dx;
            for (int x = x1; x != x2; x += sx) {
                canvas->setPixel(x, y, color);
                if (d > 0) {
                    y += sy;
                    d -= dx * 2;
                }
                d += dy * 2;
            }
        } else {
            int x = x1;
            int d = dx * 2 - dy;
            for (int y = y1; y != y2; y += sy) {
                canvas->setPixel(x, y, color);
                if (d > 0) {
                    x += sx;
                    d -= dy * 2;
                }
                d += dx * 2;
            }
        }
        canvas->setPixel(x2, y2, color);
    }

    static void draw_AA(Canvas* canvas, int x1, int y1, int x2, int y2, Color color)
    {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy, ee, xx;
        int ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + dy * dy);

        while (true) {
            canvas->setPixel(x1, y1, Color(color.value, 255 - 255 * abs(err - dx + dy) / ed));
            ee = err;
            xx = x1;
            if (ee * 2 >= -dx) {
                if (x1 == x2) break;
                if (ee + dy < ed) canvas->setPixel(x1, y1 + sy, Color(color.value, 255 - 255 * (ee + dy) / ed));
                err -= dy;
                x1 += sx;
            }
            if (ee * 2 <= dy) {
                if (y1 == y2) break;
                if (dx - ee < ed) canvas->setPixel(xx + sx, y1, Color(color.value, 255 - 255 * (dx - ee) / ed));
                err += dx;
                y1 += sy;
            }
        }
    }



    static void setPerpendicular(float* x1, float* y1, float* x2, float* y2)
    {
        float dx = *x2 - *x1;
        float dy = *y2 - *y1;

        float center_x = (*x1 + *x2) * .5f;
        float center_y = (*y1 + *y2) * .5f;

        *x1 = center_x + dy;
        *y1 = center_y - dx;
        *x2 = center_x - dy;
        *y2 = center_y + dx;
    }

    static bool intersection_line(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* ix, float* iy)
    {
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) return false;

        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;

        *ix = x1 + (x2 - x1) * t;
        *iy = y1 + (y2 - y1) * t;
        return true;
    }

    static bool intersection_segment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* ix, float* iy)
    {
        float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) return false;

        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        if (0 > t || t > 1) return false;

        float u = ((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / -den;
        if (0 > u || u > 1) return false;

        *ix = x1 + (x2 - x1) * t;
        *iy = y1 + (y2 - y1) * t;
        return true;
    }

    static bool intersection_ray(float x1, float y1, float x1_dir, float y1_dir, float x2, float y2, float x2_dir, float y2_dir, float* ix, float* iy)
    {
        float den = x1_dir * y2_dir - y1_dir * x2_dir;
        if (den == 0) return false;

        float t = ((x1 - x2) * y2_dir - (y1 - y2) * x2_dir) / den;
        if (0 > t) return false;

        float u = ((y1 - y2) * x1_dir - (x1 - x2) * y1_dir) / -den;
        if (0 > u) return false;

        *ix = x1 - x1_dir * t;
        *iy = y1 - y1_dir * t;
        return true;
    }
};



// bool clip_box(float *x1, float *y1, float *x2, float *y2, float border_top, float border_right, float border_bottom, float border_left)
// {
//     bool isClipped = false;

//     if (*y1 > border_top && *y2 > border_top) return isClipped;
//     if (*x1 > border_right && *x2 > border_right) return isClipped;
//     if (*y1 < border_bottom && *y2 < border_bottom) return isClipped;
//     if (*x1 < border_left && *x2 < border_left) return isClipped;

//     // clip top
//     if (*y1 > border_top) {
//         *x1 = *x2 - (*y2 - border_top) * ((*x2 - *x1) / (*y2 - *y1));
//         *y1 = border_top;
//         isClipped = true;
//     } else if (*y2 > border_top) {
//         *x2 = *x1 - (*y1 - border_top) * ((*x2 - *x1) / (*y2 - *y1));
//         *y2 = border_top;
//         isClipped = true;
//     }

//     // clip right
//     if (*x1 > border_right) {
//         *y1 = *y2 - (*x2 - border_right) * ((*y2 - *y1) / (*x2 - *x1));
//         *x1 = border_right;
//         isClipped = true;
//     } else if (*x2 > border_right) {
//         *y2 = *y1 - (*x1 - border_right) * ((*y2 - *y1) / (*x2 - *x1));
//         *x2 = border_right;
//         isClipped = true;
//     }

//     // clip bottom
//     if (*y1 < border_bottom) {
//         *x1 = *x2 - (*y2 - border_bottom) * ((*x2 - *x1) / (*y2 - *y1));
//         *y1 = border_bottom;
//         isClipped = true;
//     } else if (*y2 < border_bottom) {
//         *x2 = *x1 - (*y1 - border_bottom) * ((*x2 - *x1) / (*y2 - *y1));
//         *y2 = border_bottom;
//         isClipped = true;
//     }

//     // clip left
//     if (*x1 < border_left) {
//         *y1 = *y2 - (*x2 - border_left) * ((*y2 - *y1) / (*x2 - *x1));
//         *x1 = border_left;
//         isClipped = true;
//     } else if (*x2 < border_left) {
//         *y2 = *y1 - (*x1 - border_left) * ((*y2 - *y1) / (*x2 - *x1));
//         *x2 = border_left;
//         isClipped = true;
//     }

//     return isClipped;
// }
