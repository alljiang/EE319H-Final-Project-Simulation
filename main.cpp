
#include <iostream>
#include <SDL.h>
#include <windows.h>
#include <fcntl.h>

#include "LCD.h"
#include "controller.h"
#include "utils.h"
#include "UART.h"
#include "metadata.h"
#include "SRAM.h"
#include "entities.h"
#include "stage.h"

using namespace std;
using namespace chrono;

SDL_Event event;

Player* p1;
Player* p2;
Stage stage;

Kirby k1;
Kirby k2;

bool quit;

float x = 0;
float y = 0;

char strBuffer[100];
long long lastLoopMillis;

//  runs once at beginning
void startup() {
    animator_initialize();

    stage.initialize(0);

    p1 = &k1;
    p1->setPlayer(1);
    p1->setX(stage.getStartX(1));
    p1->setY(stage.getStartY(1));

    p2 = &k2;
    k2.setPlayer(2);
    p2->setX(stage.getStartX(2));
    p2->setY(stage.getStartY(2));

    UART_readCharacterSDCard(0);
}

//  continually loops
uint32_t  t1 = 0;
uint32_t tt1 = 0;
uint8_t frame = 0;
void loop() {
    if(millis() - tt1 > 1000) {
        uint32_t sum = SRAM_SPICounter + ILI9341_SPICounter;
        double max = 4000000;
        printf("SPI Bus Usage: %0.2f%\n", sum/max*100);
        SRAM_SPICounter = 0;
        ILI9341_SPICounter = 0;
        tt1 = millis();
    }
    if(millis() - t1 > 16) {
        t1 = millis();
        stage.update();
        p1->controlLoop(
                getJoystick_h(1), getJoystick_v(1),
                getBtn_a(1), getBtn_b(1),
                getBtn_l(1) || getBtn_r(1), &stage
                );

//        p2->controlLoop(
//                getJoystick_h(2), getJoystick_v(2),
//                getBtn_a(2), getBtn_b(2),
//                getBtn_l(2) || getBtn_r(2)
//                );

//        if(frame++ == 14) {
//            frame = 0;
//            SpriteSendable s;
//            s.persistent = false;
//            s.charIndex = 0;
//            s.animationIndex = 1;
//            s.x = 50;
//            s.y = 50;
//            s.frame = 0;
//            s.framePeriod = 2;
//            s.continuous = true;
//            s.layer = LAYER_CHARACTER;
//
//            UART_sendAnimation(s);
//        }

        animator_update();
    }
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

    SRAM_reset();
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
        LCD_update();
    }

    stopSDL2();

    return EXIT_SUCCESS;
}