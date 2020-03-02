//
// Created by Allen on 2/11/2020.
//

#include "UART.h"
#include "metadata.h"
#include "animator.h"
#include "utils.h"

void sleep(uint32_t ms) {
    uint32_t t1 = millis();
    while(millis() - t1 < ms) {}
}