#include <cstring> // memset
#include <iostream> // cout

#include "canvas.h"
#include "lodepng/lodepng.h"

Canvas::Canvas() { }

Canvas::Canvas(unsigned int width, unsigned int height)
{
    init(width, height);
}

Canvas::Canvas(const char* fileName)
{
    loadImage_PNG(fileName);
}

Canvas::~Canvas() { delete[] this->pixels; }

void Canvas::init(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    this->pixels = new int[this->width * this->height];
}

void Canvas::clearCanvas()
{
    memset(pixels, 0, width * height * sizeof(int));
}

void Canvas::fillCanvas(Color color)
{
    for (int i = 0; i < width * height; i++) pixels[i] = color.value;
}

void Canvas::setPixel(int x, int y, Color color)
{
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    if (color.getAlpha() == 255) {
        pixels[x + y * width] = color.value;
        return;
    }

    if (color.getAlpha() == 0) {
        return;
    }

    Color currPixel = Color(pixels[x + y * width]);
    float weight = color.getAlpha() / 255.0f;
    int r = int(currPixel.getRed()   * (1.0f - weight) + color.getRed()   * weight);
    int g = int(currPixel.getGreen() * (1.0f - weight) + color.getGreen() * weight);
    int b = int(currPixel.getBlue()  * (1.0f - weight) + color.getBlue()  * weight);
    int a = (currPixel.getAlpha() + color.getAlpha()) >> 1;

    pixels[x + y * width] = Color(r, g, b, a).value;
}

bool Canvas::getPixel(int x, int y, Color* color)
{
    if (x < 0 || x >= width || y < 0 || y >= height) return false;

    *color = Color(pixels[x + y * width]);

    return true;
}

void Canvas::setPixels(int x, int y, Canvas* canvas)
{
    for (int i = 0; i < canvas->width; i++) {
        for (int j = 0; j < canvas->height; j++) {
            setPixel(x + i, y + j, canvas->pixels[i + j * canvas->width]);
        }
    }
}

bool Canvas::getPixels(int x, int y, unsigned int w, unsigned int h, Canvas* canvas)
{
    canvas = new Canvas(w, h);

    if ((x < 0 && x + canvas->width < 0) || (x >= width && x + canvas->width > width) ||
        (y < 0 && y + canvas->height < 0) || (y >= height && x + canvas->height > height))
            return false;

    Color color;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            getPixel(x + i, y + j, &color);
            canvas->pixels[i + j * w] = color.value;
        }
    }

    return true;
}

bool Canvas::loadImage_PNG(const char* fileName)
{
    unsigned imageWidth, imageHeight, channelCount = 4;
    unsigned char *image;
    unsigned error = lodepng_decode32_file(&image, &imageWidth, &imageHeight, fileName);
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    init(imageWidth, imageHeight);
    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            this->pixels[i + j * this->width] =
                image[(i * channelCount + 0) + (this->height - 1 - j) * (this->width * channelCount)] |
                image[(i * channelCount + 1) + (this->height - 1 - j) * (this->width * channelCount)] << 8 |
                image[(i * channelCount + 2) + (this->height - 1 - j) * (this->width * channelCount)] << 16 |
                image[(i * channelCount + 3) + (this->height - 1 - j) * (this->width * channelCount)] << 24;
        }
    }

    return true;
}
