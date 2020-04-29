//
// Created by Allen on 4/28/2020.
//

#include "stagemenu.h"
#include "UART.h"
#include "metadata.h"
#include "colors_stagemenu.h"

void StageMenu::start() {
    reset();
    animator_setBackgroundColors(colors_stagemenu);
    animator_readPersistentSprite(persistentSprites[BACKGROUND_STAGEMENU], 0, 0);
    animator_readCharacterSDCard(4);
}

void StageMenu::loop(double joyH1, double joyV1, double joyH2, double joyV2, bool btnA,
        void (*transitionCall)(int8_t)) {
    double dt = 49;
    currentTime += (uint8_t)dt;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.framePeriod = 1;
    s.persistent = false;
    s.continuous = false;
    s.mirrored = false;

    //  P1 btn A press
    if(btnA) {
        int8_t selectedStage = getStage(cursorX, cursorY);
        if(selectedStage != -1) {
            (*transitionCall)(selectedStage);
            return;
        }
    }

    double joyH = joyH1 + joyH2;
    double joyV = joyV1 + joyV2;

    cursorX += STAGEMENU_CURSORSPEED * joyH;
    cursorY += STAGEMENU_CURSORSPEED * joyV;

    s.animationIndex = 4;
    s.frame = 0;
    s.x = (int16_t) cursorX;
    s.y = (int16_t) cursorY;
    s.layer = LAYER_CHARACTER;

    UART_sendAnimation(s);

    UART_commandUpdate();
}

void StageMenu::reset() {
    currentTime = 0;
    cursorX = 145;
    cursorY = 35;
}

int8_t StageMenu::getStage(double x, double y) {
//    printf("%0.0f\t%0.0f\n", x, y);
    if(y < 43 || y > 160 || x < 20 || x > 263) return -1;

    if(y > 102) {
        if(x < 100) {
            return STAGE_FINALDESTINATION;
        }
        else if(x < 182) {
            return STAGE_TOWER;
        }
        else {
            return STAGE_BATTLEFIELD;
        }
    }
    else {
        if(x < 100) {
            return STAGE_SMASHVILLE;
        }
        else if(x < 182) {
            return STAGE_EER;
        }
        else {
            return -1;
        }
    }

}