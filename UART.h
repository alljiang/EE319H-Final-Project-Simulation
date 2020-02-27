//
// Created by Allen on 2/11/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_UART_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_UART_H

#include "animator.h"
#include <cstdint>

extern void UART_readCharacterSDCard(uint8_t charIndex);
extern void UART_sendAnimation(struct SpriteSendable sendable);

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_UART_H
