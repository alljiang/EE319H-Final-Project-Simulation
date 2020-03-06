
#include <cstdlib>
#include <cstdio>
#include "SDL.h"
#include <fstream>
#include <iostream>
#include "LCD.h"
#include "metadata.h"
#include "utils.h"
#include "colors.h"

using namespace std;

#define WINDOW_SCALE 3
#define WINDOW_HEIGHT 241  // y
#define WINDOW_WIDTH 321  // x

SDL_Renderer *renderer;
SDL_Window *window;

uint32_t ILI9341_SPICounter = 0;

uint32_t screen[241][321];   // simulator only, for hitbox display
bool changed[241][321]; // simulator only, is true if hitbox is displayed

void LCD_startLCD() {
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE,
                                0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void ILI9341_drawColors(uint32_t x, uint32_t y, int32_t *rgbArr, uint16_t totalPixels) {
    for(uint32_t i = 0; i < totalPixels; i++) {
        uint32_t rgb = rgbArr[i];
        LCD_drawPixel(x+i, y, rgb, false);
    }
    ILI9341_SPICounter += 11 + 3 * totalPixels;
}

void ILI9341_fillScreen(uint32_t rgb) {
    for(int r = 0; r < 241; r++) {
        for(int c = 0; c < 321; c++) {
            LCD_drawPixel(c, r, rgb, false);
        }
    }
}

// simulator only
void LCD_drawOverlayCircle(double doublex, double doubley, uint8_t radius, uint32_t colorSub) {
    double cx = (double) doublex;
    double cy = (double) doubley;

    double radius_sq = radius * radius;

    for(int16_t y = cy + radius; y >= cy - radius; y--) {
        for(int16_t x = cx - radius; x <= cx + radius; x++) {
            double distance_sq = (x-cx)*(x-cx) + (y-cy)*(y-cy);

            if(radius_sq >= distance_sq) {
                uint32_t oldColor = screen[y][x];
                uint16_t oldRed = (oldColor >> 16u) & 0xFF;
                uint16_t oldGreen = (oldColor >> 8u) & 0xFF;
                uint16_t oldBlue = oldColor & 0xFF;

                uint16_t overlayRed = (colorSub >> 16u) & 0xFF;
                uint16_t overlayGreen = (colorSub >> 8u) & 0xFF;
                uint16_t overlayBlue = colorSub & 0xFF;

                int16_t newRed = oldRed - overlayRed;
                int16_t newGreen = oldGreen - overlayGreen;
                int16_t newBlue = oldBlue - overlayBlue;

                if(newRed < 0) newRed = 0;
                if(newGreen < 0) newGreen = 0;
                if(newBlue < 0) newBlue = 0;

//                if(newRed > 255) newRed = 255;
//                if(newGreen > 255) newGreen = 255;
//                if(newBlue > 255) newBlue = 255;

                uint32_t newRGB = ((newRed & 0xFF) << 16) + ((newGreen & 0xFF) << 8) + (newBlue & 0xFF);

                changed[y][x] = true;
                LCD_drawPixel(x, y, newRGB, true);
            }
        }
    }
}

// simulator only
void LCD_drawOverlayRectangle(double doublex, double doubley,
        uint8_t width, uint8_t height, uint32_t colorSub) {
    uint16_t cx = (uint16_t) doublex;
    uint16_t cy = (uint16_t) doubley;

    for(int16_t y = cy + height/2; y >= cy - height/2; y--) {
        for(int16_t x = cx - width/2; x <= cx + width/2; x++) {
            uint32_t oldColor = screen[y][x];
            uint16_t oldRed = (oldColor >> 16u) & 0xFF;
            uint16_t oldGreen = (oldColor >> 8u) & 0xFF;
            uint16_t oldBlue = oldColor & 0xFF;

            uint16_t overlayRed = (colorSub >> 16u) & 0xFF;
            uint16_t overlayGreen = (colorSub >> 8u) & 0xFF;
            uint16_t overlayBlue = colorSub & 0xFF;

            int16_t newRed = oldRed - overlayRed;
            int16_t newGreen = oldGreen - overlayGreen;
            int16_t newBlue = oldBlue - overlayBlue;

            if(newRed < 0) newRed = 0;
            if(newGreen < 0) newGreen = 0;
            if(newBlue < 0) newBlue = 0;

//                if(newRed > 255) newRed = 255;
//                if(newGreen > 255) newGreen = 255;
//                if(newBlue > 255) newBlue = 255;

            uint32_t newRGB = ((newRed & 0xFF) << 16) + ((newGreen & 0xFF) << 8) + (newBlue & 0xFF);

            changed[y][x] = true;
            LCD_drawPixel(x, y, newRGB, true);
        }
    }
}

void LCD_clearOverlay() {
    for(int r = 0; r < 241; r++) {
        for(int c = 0; c < 321; c++) {
            if(changed[r][c]) {
                changed[r][c] = false;
                LCD_drawPixel(c, r, screen[r][c], false);
            }
        }
    }
}

void LCD_drawPixel(uint16_t x, uint16_t y, uint32_t rgb, bool isOverlay) {
    if(x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) {
        return;
    }

    if(!isOverlay) screen[y][x] = rgb;

    y = WINDOW_HEIGHT - 1 - y;
    SDL_SetRenderDrawColor(renderer, rgb>>16, rgb>>8, rgb, 255);
    for(int i = x*WINDOW_SCALE; i < x*WINDOW_SCALE+WINDOW_SCALE; i++) {
        for(int j = y * WINDOW_SCALE; j < y*WINDOW_SCALE+WINDOW_SCALE; j++) {
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

void LCD_update() {
    SDL_RenderPresent(renderer);
}

void stopSDL2() {
    // cleanup SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}