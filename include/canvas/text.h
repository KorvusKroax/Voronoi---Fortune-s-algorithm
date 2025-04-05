#pragma once

#include <iostream>
#include "canvas.h"
#include "color.h"
#include "font.h"

struct Text
{
    // https://hackaday.io/project/6309-vga-graphics-over-spi-and-serial-vgatonic/log/20759-a-tiny-4x6-pixel-font-that-will-fit-on-almost-any-microcontroller-license-mit

    static void drawChar(Canvas* canvas, int x, int y, unsigned char asciiCode, Color color)
    {
        int charIndex = asciiCode - 32;
        for (int j = 0; j < 6; j++) {
            unsigned char pixel = 0;
            switch (j - (font4x6[charIndex] & 1)) {
                case 0: pixel = (font4x6[charIndex] >> 12) & 0xe; break;
                case 1: pixel = (font4x6[charIndex] >> 9) & 0xe; break;
                case 2: pixel = ((font4x6[charIndex] & 0x0300) >> 6) | (font4x6[charIndex] & 0x02); break;
                case 3: pixel = (font4x6[charIndex] >> 4) & 0xe; break;
                case 4: pixel = (font4x6[charIndex] >> 1) & 0xe; break;
            }

            for (int i = 0; i < 4; i++) {
                if ((pixel >> i) & 1) {
                    canvas->setPixel(x + 3 - i,  y + 5 - j, color);
                }
            }

        }
    }

    static void drawText(Canvas* canvas, int x, int y, const char* text, Color color)
    {
        int index = 0;
        while (text[index] != '\0') {
            drawChar(canvas, x + index * 4, y, text[index], color);
            index++;
        }
    }



    // static void drawChar(int x, int y, unsigned char asciiCode, Color color, Color background)
    // {
    //     int charIndex = asciiCode - 32;
    //     for (int j = 0; j < 6; j++) {
    //         unsigned char pixel = 0;
    //         switch (j - (font4x6[charIndex] & 1)) {
    //             case 0: pixel = (font4x6[charIndex] >> 12) & 0xe; break;
    //             case 1: pixel = (font4x6[charIndex] >> 9) & 0xe; break;
    //             case 2: pixel = ((font4x6[charIndex] & 0x0300) >> 6) | (font4x6[charIndex] & 0x02); break;
    //             case 3: pixel = (font4x6[charIndex] >> 4) & 0xe; break;
    //             case 4: pixel = (font4x6[charIndex] >> 1) & 0xe; break;
    //         }

    //         for (int i = 0; i < 4; i++) {
    //             setPixel(x + 3 - i,  y + 5 - j, (pixel >> i) & 1 ? color : background);
    //         }
    //     }
    // }

    // static void drawText(int x, int y, const char* text, Color color, Color background)
    // {
    //     int index = 0;
    //     while (text[index] != '\0') {
    //         drawChar(x + index * 4, y, text[index], color, background);
    //         index++;
    //     }
    // }



    static void drawChar(Canvas* canvas, int x, int y, unsigned char asciiCode, Color color, Font* font)
    {
        int charIndex = asciiCode - 32;
        for (int j = 0; j < font->height; j++) {
            for (int i = 0; i < font->width; i++) {
                if (font->charset[charIndex][i + j * font->width] & 0xff000000) {
                    canvas->setPixel(x + i, y + j, color);
                }
            }
        }
    }

    static void drawText(Canvas* canvas, int x, int y, const char* text, Color color, Font* font)
    {
        int index = 0;
        while (text[index] != '\0') {
            drawChar(canvas, x + index * font->width, y, text[index], color, font);
            index++;
        }
    }



    // static void drawChar(int x, int y, unsigned char asciiCode, Color color, Color background, Font* font)
    // {
    //     int charIndex = asciiCode - 32;
    //     for (int j = 0; j < font->height; j++) {
    //         for (int i = 0; i < font->width; i++) {
    //             setPixel(x + i, y + j, font->charset[charIndex][i + j * font->width] & 0xff000000 ? color : background);
    //         }
    //     }
    // }

    // static void drawText(int x, int y, const char* text, Color color, Color background, Font* font)
    // {
    //     int index = 0;
    //     while (text[index] != '\0') {
    //         drawChar(x + index * font->width, y, text[index], color, background, font);
    //         index++;
    //     }
    // }

private:
    static constexpr unsigned short font4x6 [96] {
        0x0000,   /*SPACE*/
        0x4908,   /*'!'*/
        0xb400,   /*'"'*/
        0xbef6,   /*'#'*/
        0x7b7a,   /*'$'*/
        0xa594,   /*'%'*/
        0x55b8,   /*'&'*/
        0x4800,   /*'''*/
        0x2944,   /*'('*/
        0x442a,   /*')'*/
        0x15a0,   /*'*'*/
        0x0b42,   /*'+'*/
        0x0050,   /*','*/
        0x0302,   /*'-'*/
        0x0008,   /*'.'*/
        0x2590,   /*'/'*/
        0x76ba,   /*'0'*/
        0x595c,   /*'1'*/
        0xc59e,   /*'2'*/
        0xc538,   /*'3'*/
        0x92e6,   /*'4'*/
        0xf33a,   /*'5'*/
        0x73ba,   /*'6'*/
        0xe590,   /*'7'*/
        0x77ba,   /*'8'*/
        0x773a,   /*'9'*/
        0x0840,   /*':'*/
        0x0850,   /*';'*/
        0x2a44,   /*'<'*/
        0x1ce0,   /*'='*/
        0x8852,   /*'>'*/
        0xe508,   /*'?'*/
        0x568e,   /*'@'*/
        0x77b6,   /*'A'*/
        0x77b8,   /*'B'*/
        0x728c,   /*'C'*/
        0xd6ba,   /*'D'*/
        0x739e,   /*'E'*/
        0x7392,   /*'F'*/
        0x72ae,   /*'G'*/
        0xb7b6,   /*'H'*/
        0xe95c,   /*'I'*/
        0x64aa,   /*'J'*/
        0xb7b4,   /*'K'*/
        0x929c,   /*'L'*/
        0xbeb6,   /*'M'*/
        0xd6b6,   /*'N'*/
        0x56aa,   /*'O'*/
        0xd792,   /*'P'*/
        0x76ee,   /*'Q'*/
        0x77b4,   /*'R'*/
        0x7138,   /*'S'*/
        0xe948,   /*'T'*/
        0xb6ae,   /*'U'*/
        0xb6aa,   /*'V'*/
        0xb6f6,   /*'W'*/
        0xb5b4,   /*'X'*/
        0xb548,   /*'Y'*/
        0xe59c,   /*'Z'*/
        0x694c,   /*'['*/
        0x9124,   /*'\'*/
        0x642e,   /*']'*/
        0x5400,   /*'^'*/
        0x001c,   /*'_'*/
        0x4400,   /*'`'*/
        0x0eae,   /*'a'*/
        0x9aba,   /*'b'*/
        0x0e8c,   /*'c'*/
        0x2eae,   /*'d'*/
        0x0ece,   /*'e'*/
        0x56d0,   /*'f'*/
        0x553B,   /*'g'*/
        0x93b4,   /*'h'*/
        0x4144,   /*'i'*/
        0x4151,   /*'j'*/
        0x97b4,   /*'k'*/
        0x4944,   /*'l'*/
        0x17b6,   /*'m'*/
        0x1ab6,   /*'n'*/
        0x0aaa,   /*'o'*/
        0xd6d3,   /*'p'*/
        0x7667,   /*'q'*/
        0x1790,   /*'r'*/
        0x0f38,   /*'s'*/
        0x9a8c,   /*'t'*/
        0x16ae,   /*'u'*/
        0x16ba,   /*'v'*/
        0x16f6,   /*'w'*/
        0x15b4,   /*'x'*/
        0xb52b,   /*'y'*/
        0x1c5e,   /*'z'*/
        0x6b4c,   /*'{'*/
        0x4948,   /*'|'*/
        0xc95a,   /*'}'*/
        0x5400,   /*'~'*/
        0x56e2    /*''*/
    };
};
