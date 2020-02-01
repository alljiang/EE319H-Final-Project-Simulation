#include <thread>
#include <iostream>
#include <SDL.h>
#include <synchapi.h>
#include <cstdlib>

using namespace std;

#define WINDOW_HEIGHT 960  // 240 * 4, y
#define WINDOW_WIDTH 1280  // 320 * 4, x
#define NUM_THREADS 5

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;

void startLCD();
void drawPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);

void loop() {
    Sleep(500);
    drawPixel(0, 50,255,0,0);
}

int main(int argc, char *argv[]) {
    startLCD();

    thread t1(loop);
    t1.join();

    while(true) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void startLCD() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void drawPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    if(x >= 320 || y >= 240) {
        return;
    }

    y = 239 - y;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for(int i = x*4; i < x*4+4; i++) {
        for(int j = y * 4; j < y*4+4; j++) {
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
    SDL_RenderPresent(renderer);
}