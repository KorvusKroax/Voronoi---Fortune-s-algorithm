#include <algorithm>
#include <iostream>
#include <vector>

#include "voronoi.h"
#include "circle.h"
#include "line.h"

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





    std::vector<Site> beachLine;

    for (int i = 0; i < this->siteCount; i++) {
        if (this->sites[i].y > sweepLine_y) break;

        if (beachLine.empty()) {
            beachLine.push_back(this->sites[i]);
            continue;
        }

        int index = 0;
        for (int j = 1; j < beachLine.size(); j++) {
            if (this->sites[i].x < getIntersect_x(beachLine[j - 1].x, beachLine[j - 1].y, beachLine[j].x, beachLine[j].y, sweepLine_y)) {
                index = j - 1;
                break;
            }
        }

        // insert the new site into the D list
        beachLine.insert(beachLine.begin() + index, beachLine[index]);
        beachLine.insert(beachLine.begin() + index + 1, sites[i]);

        // if (i < 3) continue;
        // draw circles
        // ...
    }





    if (!beachLine.empty()) {
        for (int i = 0; i < beachLine.size(); i++) {
            std::cout << "beachLine[" << i << "] = (x" << beachLine[i].x << ", y" << beachLine[i].y << ")" << std::endl;
        }
        std::cout << std::endl;
    }

    if (!beachLine.empty()) {

        for (int i = 0; i < beachLine.size(); i++) {
            drawParabola(canvas, beachLine[i].x, beachLine[i].y, sweepLine_y, 0, canvas->width - 1, EGA_DARK_GREY);
        }

        float rightX, leftX = 0;
        for (int i = 0; i < beachLine.size(); i++) {
            rightX = (i < beachLine.size() - 1) ?
                getIntersect_x(beachLine[i].x, beachLine[i].y, beachLine[i + 1].x, beachLine[i + 1].y, sweepLine_y) :
                canvas->width - 1;
            drawParabola(canvas, beachLine[i].x, beachLine[i].y, sweepLine_y, leftX, rightX, EGA_GREEN);
            leftX = rightX;
        }

    }
}

void Voronoi::drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y, int leftX, int rightX, Color color)
{
    if (yf == sweepLine_y) return;
    for (int x = leftX; x < rightX; x++) {
        // int y = (1.0f / (2 * (yf - sweepLine_y))) * ((x - xf) * (x - xf)) + ((yf + sweepLine_y) / 2);
        int y = (((x - xf) * (x - xf)) / (2 * (yf - sweepLine_y))) + ((yf + sweepLine_y) / 2);
        canvas->setPixel(x, y, color);
    }
}

double Voronoi::getIntersect_x(double siteLeft_x, double siteLeft_y, double siteRight_x, double siteRight_y, double sweepLine_y)
{
    if (siteLeft_y == sweepLine_y) return siteLeft_x;
    if (siteRight_y == sweepLine_y) return siteRight_x;

    double dp = 2.0 * (siteLeft_y - sweepLine_y);
    double a1 = 1.0 / dp;
    double b1 = -2.0 * siteLeft_x / dp;
    double c1 = sweepLine_y + dp / 4.0 + (siteLeft_x * siteLeft_x) / dp;

           dp = 2.0 * (siteRight_y - sweepLine_y);
    double a2 = 1.0 / dp;
    double b2 = -2.0 * siteRight_x / dp;
    double c2 = sweepLine_y + dp / 4.0 + (siteRight_x * siteRight_x) / dp;

    double a = a1 - a2;
    double b = b1 - b2;
    double c = c1 - c2;

    double sqrtDiscriminant = std::sqrt(b * b - 4.0 * a * c);
    double x1 = (-b + sqrtDiscriminant) / (2 * a);
    double x2 = (-b - sqrtDiscriminant) / (2 * a);

    return siteLeft_y > siteRight_y ? std::max(x1, x2) : std::min(x1, x2);
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
