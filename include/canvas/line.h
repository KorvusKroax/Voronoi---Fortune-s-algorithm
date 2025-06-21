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

    static bool getNormalizedDirection(float x1, float y1, float x2, float y2, float* dir_x, float* dir_y)
    {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        if (length == 0) return false;

        *dir_x = dx / length;
        *dir_y = dy / length;
        return true;
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

        float t = ((x2 - x1) * y2_dir - (y2 - y1) * x2_dir) / den;
        if (0 > t) return false;

        float u = (x1_dir * (y2 - y1) - y1_dir * (x2 - x1)) / -den;
        if (0 > u) return false;

        *ix = x1 + x1_dir * t;
        *iy = y1 + y1_dir * t;
        return true;
    }

    static void boundIntersection_ray(float x1, float y1, float dir_x, float dir_y, float min_x, float min_y, float max_x, float max_y, float* ix, float* iy)
    {
        float tMin = INFINITY;

        if (dir_x != 0) {
            float x = (dir_x < min_x ? min_x : max_x);
            float t = (x - x1) / dir_x;
            if (t > 0) {
                float y = y1 + t * dir_y;
                if (y >= min_y && y < max_y) {
                    tMin = t;
                    *ix = x;
                    *iy = y;
                }
            }
        }

        if (dir_y != 0) {
            float y = (dir_y < min_y ? min_y : max_y);
            float t = (y - y1) / dir_y;
            if (t > 0 && t < tMin) {
                float x = x1 + t * dir_x;
                if (x >= min_x && x < max_x) {
                    tMin = t;
                    *ix = x;
                    *iy = y;
                }
            }
        }
    }

    static bool boundClip_segment(float* x1, float* y1, float* x2, float* y2, float xmin, float ymin, float xmax, float ymax)
    {
        float p[4], q[4];
        p[0] = -(x2 - x1); q[0] = *x1 - xmin;
        p[1] =  (x2 - x1); q[1] = xmax - *x1;
        p[2] = -(y2 - y1); q[2] = *y1 - ymin;
        p[3] =  (y2 - y1); q[3] = ymax - *y1;

        float u1 = 0.0f;
        float u2 = 1.0f;
        for (int i = 0; i < 4; i++) {
            if (p[i] == 0) {
                if (q[i] < 0) return false; // Line is parallel and outside
            } else {
                float t = q[i] / p[i];
                if (p[i] < 0) {
                    if (t > u2) return false;
                    if (t > u1) u1 = t;
                } else {
                    if (t < u1) return false;
                    if (t < u2) u2 = t;
                }
            }
        }

        float nx1 = *x1 + u1 * (*x2 - *x1);
        float ny1 = *y1 + u1 * (*y2 - *y1);
        float nx2 = *x1 + u2 * (*x2 - *x1);
        float ny2 = *y1 + u2 * (*y2 - *y1);

        *x1 = nx1;
        *y1 = ny1;
        *x2 = nx2;
        *y2 = ny2;
        return true;
    }
};
