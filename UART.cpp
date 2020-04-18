//
// Created by Allen on 2/11/2020.
//

#include <cstdio>
#include "UART.h"
#include "metadata.h"
#include "animator.h"

void UART_sendAnimation(struct SpriteSendable sendable) {
    uint8_t animation_charIndex = sendable.charIndex;
    uint8_t animation_animationIndex = sendable.animationIndex;
    int16_t x = sendable.x;
    int16_t y = sendable.y;
    uint8_t frame = sendable.frame;
    uint8_t persistent = sendable.persistent;
    uint8_t layer = sendable.layer;
    uint8_t continuous = sendable.continuous;
    uint8_t framePeriod = sendable.framePeriod;
    uint8_t mirrored = sendable.mirrored;

    animator_animate(animation_charIndex, animation_animationIndex,
            x, y, frame, layer, persistent, continuous, framePeriod, mirrored);
}

void UART_readCharacterSDCard(uint8_t charIndex) {
    animator_readCharacterSDCard(charIndex);
}