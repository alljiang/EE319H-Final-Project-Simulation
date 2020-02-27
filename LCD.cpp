
#include <cstdlib>
#include <cstdio>
#include "SDL.h"
#include <fstream>
#include <iostream>
#include "LCD.h"
#include "metadata.h"
#include "utils.h"

using namespace std;

#define WINDOW_SCALE 3
#define WINDOW_HEIGHT 241  // y
#define WINDOW_WIDTH 321  // x

SDL_Renderer *renderer;
SDL_Window *window;

void LCD_startLCD() {
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE,
                                0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void ILI9341_drawColors_indexed(uint32_t x, uint32_t y, int16_t *rgbIndex, uint16_t totalPixels) {
    for(int i = 0; i < totalPixels; i++) {
        uint32_t rgb = colors[rgbIndex[i]];
        printf("%d ", rgbIndex[i]);
        LCD_drawPixel(x+i, y, rgb);
    }
    printf("\n");
}

void LCD_drawPixel(uint16_t x, uint16_t y, uint32_t rgb) {
    if(x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) {
        return;
    }

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