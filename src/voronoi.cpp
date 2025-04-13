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
    for (int i = 0; i < this->siteCount; i++) {
        Circle::draw_filled(canvas, this->sites[i].x, this->sites[i].y, 1, EGA_RED);
    }

    Line::draw(canvas, 0, sweepLine_y, canvas->width - 1, sweepLine_y, EGA_GREY);





    std::vector<Site> D;

    for (int i = 0; i < this->siteCount; i++) {
        if (this->sites[i].y > sweepLine_y) break;

        drawParabola(canvas, this->sites[i].x, this->sites[i].y, sweepLine_y);



        if (D.empty()) {
            D.push_back(this->sites[i]);
            continue;
        }



        // find the closest parabola that intersects with the current site's parabola
        int x = this->sites[i].x;
        float yd = this->sites[i].y;
        float minDist = std::numeric_limits<float>::max();
        int siteIndex = -1;
        for (int j = 0; j < D.size(); j++) {
            float xf = D[j].x;
            float yf = D[j].y;
            int y = yd - ((((x - xf) * (x - xf)) / (2 * (yf - yd))) + ((yf + yd) / 2));
            if (y < minDist) {
                minDist = y;
                siteIndex = j;
            }
        }

        if (siteIndex == -1) {
            std::cout << "Error: siteIndex is -1" << std::endl;
            return;
        }

        // insert the new site into the D list
        D.insert(D.begin() + siteIndex, D[siteIndex]);
        D.insert(D.begin() + siteIndex + 1, sites[i]);

        // if (i < 3) continue;
        // draw circles
    }

    if (D.empty()) {
        std::cout << "D is empty" << std::endl;
    } else {
        for (int j = 0; j < D.size(); j++) {
            std::cout << "D[" << j << "] = (x" << D[j].x << ", y" << D[j].y << ")" << std::endl;
        }
        std::cout << std::endl;
    }
}

void Voronoi::drawParabola(Canvas* canvas, int xf, int yf, int sweepLine_y)
{
    float yd = sweepLine_y;
    for (int x = 0; x < canvas->width; x++) {
        // int y = (1.0f / (2 * (yf - yd))) * ((x - xf) * (x - xf)) + ((yf + yd) / 2);
        int y = (((x - xf) * (x - xf)) / (2 * (yf - yd))) + ((yf + yd) / 2);
        canvas->setPixel(x, y, EGA_GREEN);
    }
}