
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
#include "colors_finaldest.h"
#include "colors_towerback.h"

using namespace std;
using namespace chrono;

SDL_Event event;

Player* p1;
Player* p2;
Stage stage;
HitboxManager hitboxManager;

Kirby k1;
Kirby k2;

bool quit;

float x = 0;
float y = 0;

const bool PLAYER2 = true;
const bool HITBOXOVERLAY = false;
const double UPDATERATE = 20;

const uint8_t stageToPlay = STAGE_FINALDESTINATION;
//const uint8_t stageToPlay = STAGE_TOWER;

//  runs once at beginning
void startup() {
    animator_initialize();

    if(stageToPlay == STAGE_FINALDESTINATION) animator_setBackgroundColors(colors_finaldest);
    else if(stageToPlay == STAGE_TOWER) animator_setBackgroundColors(colors_towerback);
    stage.initialize(stageToPlay);

    p1 = &k1;
    p1->setPlayer(1);
    p1->setX(stage.getStartX(1));
    p1->setY(stage.getStartY(1));

    p2 = &k2;
    k2.setPlayer(2);
    p2->setX(stage.getStartX(2));
    p2->setY(stage.getStartY(2));

    if(PLAYER2) hitboxManager.initialize(p1, p2);
    else hitboxManager.initialize(p1);

    UART_readCharacterSDCard(0);

    printf("SRAM Used: %0.1f%\n", getCurrentMemoryLocation() / (1024.*1024) * 100);
}

//  continually loops
uint32_t  t1 = 0;
uint32_t tt1 = 0;
uint8_t frame = 0;
void loop() {
//    if(millis() - tt1 > 1000) {
//        uint32_t sum = SRAM_SPICounter + ILI9341_SPICounter;
//        double max = 1000000;
//        printf("SPI Bus Usage: %0.2f%\n", sum/max*100);
//        SRAM_SPICounter = 0;
//        ILI9341_SPICounter = 0;
//        tt1 = millis();
//    }
    if(millis() - t1 > 1./UPDATERATE*1000) {
        uint32_t sum = SRAM_SPICounter + ILI9341_SPICounter;
        double max = 1000000/20.;
//        printf("SPI Bus Usage: %0.2f%\n", sum/max*100);
        SRAM_SPICounter = 0;
        ILI9341_SPICounter = 0;

        t1 = millis();
        stage.update();
        p1->controlLoop(
                getJoystick_h(1), getJoystick_v(1),
                getBtn_a(1), getBtn_b(1),
                getBtn_l(1) || getBtn_r(1), &stage,
                &hitboxManager
                );

        if(PLAYER2) {
            p2->controlLoop(
                    getJoystick_h(2), getJoystick_v(2),
                    getBtn_a(2), getBtn_b(2),
                    getBtn_l(2) || getBtn_r(2), &stage,
                    &hitboxManager
            );
        }

        if(HITBOXOVERLAY) hitboxManager.clearHitboxOverlay();
        animator_update();
        if(HITBOXOVERLAY) hitboxManager.displayHitboxesOverlay();

        hitboxManager.checkCollisions();
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
