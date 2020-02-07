
#include <iostream>
#include <SDL.h>

#include "animator.h"
#include "LCD.h"
#include "controller.h"
#include "utils.h"

#include <windows.h>
#include <fcntl.h>

using namespace std;
using namespace chrono;

SDL_Event event;

bool quit;

float x = 0;
float y = 0;

char strBuffer[100];
long long lastLoopMillis = millis();

//  runs once at beginning
void startup() {
    animator_readCharacterSDCard(0);
}

//  continually loops
void loop() {
    float dt = millis() - lastLoopMillis;
    lastLoopMillis = millis();

    float pps = 100.;
    x += getJoystick_h(1) * pps * dt / 1000.;
    y += getJoystick_v(1) * pps * dt / 1000.;

    LCD_drawPixel(x, y, 0x00FF00);
    LCD_update();
}

int main(int argc, char *argv[]) {
    LCD_startLCD();

    // Print console setup
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

    startup();

    //  wait for window to close
    while (!quit)
    {
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
