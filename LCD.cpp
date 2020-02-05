
#include <cstdlib>
#include <cstdio>
#include "SDL.h"
#include <fstream>
#include <iostream>
#include "LCD.h"

using namespace std;

#define WINDOW_SCALE 3
#define WINDOW_HEIGHT 240  // y
#define WINDOW_WIDTH 320  // x

SDL_Renderer *renderer;
SDL_Window *window;

void LCD_startLCD() {
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE,
                                0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void LCD_drawPixel(uint16_t x, uint16_t y, uint32_t rgb) {
    if(x >= 320 || y >= 240) {
        return;
    }

    y = 239 - y;
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