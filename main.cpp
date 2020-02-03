#include <thread>
#include <iostream>
#include <SDL.h>
#include <cstdlib>
#include <synchapi.h>
#include <chrono>
#include <cstdio>

using namespace std;
using namespace chrono;

#define WINDOW_SCALE 3
#define WINDOW_HEIGHT 240  // y
#define WINDOW_WIDTH 320  // x
#define NUM_THREADS 5

void startLCD();
void drawPixel(uint16_t x, uint16_t y, uint32_t rgb);
void print(char *str);
long long int millis();

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
bool quit;

float joystick_h = 0;
float joystick_v = 0;
bool btn_a;
bool btn_b;
bool btn_start;
bool btn_l;
bool btn_r;

uint32_t x = 0;
uint32_t y = 0;

char strBuffer[100];
char *windowTitle;

//  runs once at beginning
void startup() {

}

//  continually loops
void loop() {
    sprintf(strBuffer, "%d+", millis());
    print(strBuffer);
    drawPixel(x, y, 0xFF0000);
}

int main(int argc, char *argv[]) {
    startLCD();

    startup();
    //  wait for window to close
    while (!quit)
    {
        SDL_SetWindowTitle(window, windowTitle);
        SDL_PollEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_LEFT:
                        joystick_h = -0.3;
                        break;
                    case SDLK_RIGHT:
                        joystick_h = 0.3;
                        break;
                    case SDLK_UP:
                        joystick_v = 1.0;
                        break;
                    case SDLK_DOWN:
                        joystick_v = -1.0;
                        break;
                    case SDLK_w:
                        break;
                    case SDLK_s:
                        break;
                    case SDLK_a:
                        break;
                    case SDLK_d:
                        break;
                    case SDLK_v:
                        break;
                    case SDLK_b:
                        break;
                    case SDLK_SPACE:
                        break;
                    case SDLK_RSHIFT:
                        break;
                    case SDLK_LSHIFT:
                    break;
                }
        }

        loop();

//        SDL_RenderClear(renderer);
//        SDL_RenderPresent(renderer);
    }

    // cleanup SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void startLCD() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE,
            0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void drawPixel(uint16_t x, uint16_t y, uint32_t rgb) {
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
//    SDL_RenderPresent(renderer);

}

long long int millis() {
    return duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    ).count();
}

void print(char str[]) {
    windowTitle = str;
}