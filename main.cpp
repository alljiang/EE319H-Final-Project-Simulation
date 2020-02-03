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

float x = 0;
float y = 0;

char strBuffer[100];
char *windowTitle;
long long lastLoopMillis = millis();

//  runs once at beginning
void startup() {
    
}

//  continually loops
void loop() {
    float dt = millis() - lastLoopMillis;
    lastLoopMillis = millis();

//    sprintf(strBuffer, "%d+", millis());
//    print(strBuffer);

    float pps = 100.;
    x += joystick_h * pps * dt / 1000.;
    y += joystick_v * pps * dt / 1000.;

    sprintf(strBuffer, "%f", x);
    print(strBuffer);

    drawPixel(x, y, 0xFF0000);
}

int main(int argc, char *argv[]) {
    startLCD();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    startup();
    //  wait for window to close
    while (!quit)
    {
        SDL_SetWindowTitle(window, windowTitle);
        SDL_PollEvent(&event);

        joystick_h = 0;
        joystick_v = 0;
        btn_a = false;
        btn_b = false;
        btn_start = false;
        btn_l = false;
        btn_r = false;
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
        }

        if(keys[SDL_SCANCODE_LEFT]) {
            joystick_h += -0.3;
        }
        if(keys[SDL_SCANCODE_RIGHT]) {
            joystick_h += 0.3;
        }
        if(keys[SDL_SCANCODE_UP]) {
            joystick_v += 0.3;
        }
        if(keys[SDL_SCANCODE_DOWN]) {
            joystick_v -= 0.3;
        }
        if(keys[SDL_SCANCODE_W]) {
            joystick_v += 1.0;
        }
        if(keys[SDL_SCANCODE_S]) {
            print("S");
            joystick_v += -1.0;
        }
        if(keys[SDL_SCANCODE_A]) {
            joystick_h += -1.0;
        }
        if(keys[SDL_SCANCODE_D]) {
            joystick_h += 1.0;
        }
        if(keys[SDL_SCANCODE_LSHIFT]) {
            btn_l = true;
        }
        if(keys[SDL_SCANCODE_RSHIFT]) {
            btn_r = true;
        }
        if(keys[SDL_SCANCODE_SPACE]) {
            btn_start = true;
        }

        loop();
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
    SDL_RenderPresent(renderer);

}

long long int millis() {
    return duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    ).count();
}

void print(char str[]) {
    windowTitle = str;
}