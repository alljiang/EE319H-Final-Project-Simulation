//
// Created by Allen on 2/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_ANIMATOR_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_ANIMATOR_H

#include <cstdint>

extern uint32_t readNextNumber(char delimeter, char* buf);
extern char readUntil(char delimeter, char* buf);

extern void update();

extern void animator_animate(uint8_t animation_charIndex, uint8_t animation_animationIndex,
                    uint8_t x, uint16_t y, uint8_t frame, uint8_t persistent, uint8_t layer);

extern void animator_readCharacterSDCard(uint8_t charIndex);

extern void animator_initialize();

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_ANIMATOR_H
