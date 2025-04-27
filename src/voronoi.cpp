#include <algorithm>
#include <iostream>
#include <vector>

#include "circle.h"
#include "line.h"

#include "voronoi.h"
#include "beachline_parabola.h"
#include "halfedge.h"

Voronoi::Voronoi(unsigned int width, unsigned int height, int siteCount, Site* sites)
{
    this->width = width;
    this->height = height;
    this->siteCount = siteCount;
    this->sites = sites;

    std::sort(this->sites, this->sites + this->siteCount,
        [](Site const & a, Site const & b) -> bool {
            return a.y < b.y;
        }
    );
}

Voronoi::~Voronoi() { }

void Voronoi::show(Canvas* canvas, int sweepLine_y)
{
    // sites
    for (int i = 0; i < this->siteCount; i++) {
        Circle::draw_filled(canvas, this->sites[i].x, this->sites[i].y, 1, EGA_RED);
    }

    // sweep line
    Line::draw(canvas, 0, sweepLine_y, canvas->width - 1, sweepLine_y, EGA_GREY);





    std::vector<BeachlineParabola> beachLine;
    for (int i = 0; i < this->siteCount; i++) {
        if (this->sites[i].y > sweepLine_y) break;

        if (beachLine.empty()) {
            beachLine.push_back(BeachlineParabola(this->sites[i]));
            continue;
        }

        int index = 0;
        for (int j = 0; j < beachLine.size() - 1; j++) {
            if (this->sites[i].x < getIntersect_x(beachLine[j].site.x, beachLine[j].site.y, beachLine[j + 1].site.x, beachLine[j + 1].site.y, sweepLine_y)) {
                index = j;
                break;
            }
        }

        // insert the new site into the beachLine
        float dx = beachLine[index].site.x - this->sites[i].x;
        float dy = beachLine[index].site.y - this->sites[i].y;
        float edgeStart_y = (float(dx * dx) / (2 * dy)) + ((beachLine[index].site.y + this->sites[i].y) >> 1);

        beachLine.insert(beachLine.begin() + index, beachLine[index]);
        beachLine.insert(beachLine.begin() + index + 1,
            BeachlineParabola(this->sites[i],
                new HalfEdge(this->sites[i].x, edgeStart_y, -dx / dy),
                new HalfEdge(this->sites[i].x, edgeStart_y, dx / -dy)
            )
        );

        // if (i < 3) continue;
        // draw circles
        // ...
    }



    if (beachLine.empty()) return;

    // // info to console
    // for (int i = 0; i < beachLine.size(); i++) {
    //     std::cout << "beachLine[" << i << "] = (x" << beachLine[i].site.x << ", y" << beachLine[i].site.y << ")" << std::endl;
    // }
    // std::cout << std::endl;

    // full parabolas
    for (int i = 0; i < beachLine.size(); i++) {
        drawParabola(canvas, beachLine[i].site.x, beachLine[i].site.y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
    }

    float max_x, min_x = 0;
    for (int i = 0; i < beachLine.size(); i++) {
        max_x = (i < beachLine.size() - 1) ?
            getIntersect_x(beachLine[i].site.x, beachLine[i].site.y, beachLine[i + 1].site.x, beachLine[i + 1].site.y, sweepLine_y) :
            canvas->width - 1;

        // beachline
        drawParabola(canvas, beachLine[i].site.x, beachLine[i].site.y, sweepLine_y, min_x, max_x, EGA_GREEN);

        // halfedges
        if (beachLine[i].halfEdge_left != nullptr) {
            float dx = min_x - beachLine[i].halfEdge_left->x;
            Line::draw(canvas,
                beachLine[i].halfEdge_left->x,
                beachLine[i].halfEdge_left->y,
                beachLine[i].halfEdge_left->x + dx,
                beachLine[i].halfEdge_left->y + dx * beachLine[i].halfEdge_left->slope,
                EGA_CYAN
            );
            canvas->setPixel(beachLine[i].halfEdge_left->x, beachLine[i].halfEdge_left->y, EGA_WHITE);
        }
        if (beachLine[i].halfEdge_right != nullptr) {
            float dx = max_x - beachLine[i].halfEdge_right->x;
            Line::draw(canvas,
                beachLine[i].halfEdge_right->x,
                beachLine[i].halfEdge_right->y,
                beachLine[i].halfEdge_right->x + dx,
                beachLine[i].halfEdge_right->y + dx * beachLine[i].halfEdge_right->slope,
                EGA_ORANGE
            );
            canvas->setPixel(beachLine[i].halfEdge_right->x, beachLine[i].halfEdge_right->y, EGA_WHITE);
        }

        min_x = max_x;
    }
}

void Voronoi::drawParabola(Canvas* canvas, int focus_x, int focus_y, int sweepLine_y, int min_x, int max_x, Color color)
{
    if (focus_y == sweepLine_y) return;
    for (int x = min_x; x < max_x; x++) {
        // int y = (1.0f / (2 * (focus_y - sweepLine_y))) * ((x - focus_x) * (x - focus_x)) + ((focus_y + sweepLine_y) / 2);
        int y = (float((x - focus_x) * (x - focus_x)) / (2 * (focus_y - sweepLine_y))) + ((focus_y + sweepLine_y) >> 1);
        canvas->setPixel(x, y, color);
    }
}

double Voronoi::getIntersect_x(double site_x, double site_y, double nextSite_x, double nextSite_y, double sweepLine_y)
{
    if (site_y == sweepLine_y) return site_x;
    if (nextSite_y == sweepLine_y) return nextSite_x;

    double dp = 2.0 * (site_y - sweepLine_y);
    double a1 = 1.0 / dp;
    double b1 = -2.0 * site_x / dp;
    double c1 = sweepLine_y + dp / 4.0 + (site_x * site_x) / dp;

           dp = 2.0 * (nextSite_y - sweepLine_y);
    double a2 = 1.0 / dp;
    double b2 = -2.0 * nextSite_x / dp;
    double c2 = sweepLine_y + dp / 4.0 + (nextSite_x * nextSite_x) / dp;

    double a = a1 - a2;
    double b = b1 - b2;
    double c = c1 - c2;

    double sqrtDiscriminant = std::sqrt(b * b - 4.0 * a * c);
    double x1 = (-b + sqrtDiscriminant) / (2 * a);
    double x2 = (-b - sqrtDiscriminant) / (2 * a);

    return site_y > nextSite_y ? std::max(x1, x2) : std::min(x1, x2);
}




// public static float GetIntersectX(Vector2 Left, Vector2 Right, float SweepLine)
// {
//     if (Left.y == SweepLine) return Left.x;
//     if (Right.y == SweepLine) return Right.x;

//     // edge
//     Vector2 edgeStart = (Left.y > Right.y) ?
//         new Vector2(Left.x, GetValueY(Right, Left.y, Left.x)) :
//         new Vector2(Right.x, GetValueY(Left, Right.y, Right.x));
//     Vector2 length = Left - Right;
//     Vector2 direction = new Vector2(length.y, -length.x);
//     float M = -1.0f / (length.y / length.x);
//     float C = edgeStart.y - M * edgeStart.x;

//     // parabola
//     float k = (Left.y + SweepLine) * 0.5f;
//     float p = (Left.y - SweepLine) * 0.5f;
//     float a = 1.0f / (4.0f * p);
//     float b = -Left.x / (2.0f * p);
//     float c = (Left.x * Left.x / (4.0f * p)) + k;

//     float aa = a;
//     float bb = b - M;
//     float cc = c - C;
//     float discriminant = bb * bb - (4.0f * aa * cc);
//     float x1 = (-bb + Mathf.Sqrt(discriminant)) / (2.0f * aa);
//     float x2 = (-bb - Mathf.Sqrt(discriminant)) / (2.0f * aa);
//     float min = x1 < x2 ? x1 : x2;
//     float max = x1 > x2 ? x1 : x2;

//     float x = direction.x < 0 ? min : max;

//     return x;
// }

// public static float GetValueY(Vector2 Focus, float SweepLine, float X)
// {
//     float k = (Focus.y + SweepLine) * 0.5f;
//     float p = (Focus.y - SweepLine) * 0.5f;
//     float a = 1.0f / (p * 4.0f);
//     float b = -Focus.x / (p * 2.0f);
//     float c = (Focus.x * Focus.x / (p * 4.0f)) + k;

//     float y = a * X * X + b * X + c;

//     return y;
// }
