#pragma once

#include <iostream> // cout

#include "lodepng/lodepng.h"

struct Font
{
    unsigned int width, height, count;
    int** charset;

    Font() { }

    Font(const char* fileName, int fontSheetGridColumns, int fontSheetGridRows, int charWidth, int charHeight)
    {
        loadFont(fileName, fontSheetGridColumns, fontSheetGridRows, charWidth, charHeight);
    }

    ~Font()
    {
        for (int i = 0; i < this->count; i++) {
            delete[] charset[i];
        }
        delete[] charset;
    }

    bool loadFont(const char* fileName, int fontSheetGridColumns, int fontSheetGridRows, int charWidth, int charHeight)
    {
        unsigned fontSheetWidth, fontSheetHeight, channelCount = 4;
        unsigned char* fontSheet;
        unsigned error = lodepng_decode32_file(&fontSheet, &fontSheetWidth, &fontSheetHeight, fileName);
        if (error) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            return false;
        }

        this->width = charWidth;
        this->height = charHeight;
        this->count = fontSheetGridColumns * fontSheetGridRows;

        this->charset = new int*[this->count];

        for (int row = 0; row < fontSheetGridRows; row++) {
            for (int col = 0; col < fontSheetGridColumns; col++) {
                int charIndex = col + row * fontSheetGridColumns;
                this->charset[charIndex] = new int[this->width * this->height];
                int charPos = (col * this->width * channelCount) + row * this->height * (fontSheetWidth * channelCount);
                for (int y = 0; y < this->height; y++) {
                    for (int x = 0; x < this->width; x++) {
                        this->charset[charIndex][x + (this->height - 1 - y) * this->width] =
                            fontSheet[charPos + (x * channelCount + 0) + y * (fontSheetWidth * channelCount)] |
                            fontSheet[charPos + (x * channelCount + 1) + y * (fontSheetWidth * channelCount)] << 8 |
                            fontSheet[charPos + (x * channelCount + 2) + y * (fontSheetWidth * channelCount)] << 16 |
                            fontSheet[charPos + (x * channelCount + 3) + y * (fontSheetWidth * channelCount)] << 24;
                    }
                }
            }
        }

        return true;
    }
};
