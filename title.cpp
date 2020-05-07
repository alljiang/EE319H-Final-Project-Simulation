//
// Created by Allen on 5/5/2020.
//

#include "title.h"
#include "UART.h"
#include "metadata.h"
#include "colors_title.h"

void Title::start() {
    reset();
    animator_setBackgroundColors(colors_title);
    animator_readPersistentSprite(persistentSprites[BACKGROUND_TITLESCREEN], 0, 0);
    animator_readCharacterSDCard(4);
}

void Title::loop(bool buttonPressed, void (*transitionCall)(void)) {

    if(buttonPressed) {
        (*transitionCall)();
        return;
    }

    int8_t frameExtension = 10;
    if(frameLengthCounter++ >= frameExtension) {
        frameIndex++;
        frameLengthCounter = 0;
    }
    frameIndex %= 2;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.framePeriod = 1;
    s.animationIndex = 6;
    s.frame = frameIndex;
    s.persistent = false;
    s.continuous = false;
    s.mirrored = false;
    s.x = TITLE_MESSAGE_X;
    s.y = TITLE_MESSAGE_Y;

    UART_sendAnimation(s);

    UART_commandUpdate();
}

void Title::reset() {
    frameIndex = 0;
    frameLengthCounter = 0;
}