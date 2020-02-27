//
// Created by Allen on 2/11/2020.
//

#include "UART.h"
#include "metadata.h"
#include "animator.h"

void UART_sendAnimation(struct SpriteSendable sendable) {
    uint8_t animation_charIndex = sendable.charIndex;
    uint8_t animation_animationIndex = sendable.animationIndex;
    uint8_t x = sendable.x;
    uint16_t y = sendable.y;
    uint8_t frame = sendable.frame;
    uint8_t persistent = sendable.persistent;
    uint8_t layer = sendable.layer;

    animator_animate(animation_charIndex, animation_animationIndex,
            x, y, frame, layer, persistent);
}

void UART_readCharacterSDCard(uint8_t charIndex) {
    /*
     *  TODO: Simulate this as an actual UART transfer
     */
    animator_readCharacterSDCard(charIndex);
}