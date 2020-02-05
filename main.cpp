
#include <iostream>
#include <cstdlib>
#include <synchapi.h>
#include <chrono>
#include <cstdio>
#include <SDL.h>

#include "SRAM.h"
#include "SDCard.h"
#include "animator.h"
#include "LCD.h"
#include "controller.h"

using namespace std;
using namespace chrono;

SDL_Event event;
long long int millis();

bool quit;

float x = 0;
float y = 0;

uint8_t strBuffer[100];
long long lastLoopMillis = millis();


//  runs once at beginning
void startup() {
    char name[] = "kirby";
    animator_readAnimations(name);
}

//  continually loops
void loop() {
    float dt = millis() - lastLoopMillis;
    lastLoopMillis = millis();

//    sprintf(strBuffer, "%d+", millis());
//    print(strBuffer);

    float pps = 100.;
    x += getJoystick_h(1) * pps * dt / 1000.;
    y += getJoystick_v(1) * pps * dt / 1000.;

    print(strBuffer);

    LCD_drawPixel(x, y, 0x00FF00);
    LCD_update();
}

int main(int argc, char *argv[]) {
    LCD_startLCD();
    const uint8_t *keys = SDL_GetKeyboardState(NULL);

    startup();

    //  wait for window to close
    while (!quit)
    {
        updateWindowTitle();
        SDL_PollEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
        }

        controller_updateController();
        loop();
    }

    stopSDL2();

    return EXIT_SUCCESS;
}

long long int millis() {
    return duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    ).count();
}
