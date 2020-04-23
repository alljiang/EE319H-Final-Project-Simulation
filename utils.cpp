//
// Created by Allen on 2/11/2020.
//

#include "UART.h"
#include "metadata.h"
#include "animator.h"
#include "utils.h"

bool seedSet = false;

void sleep(uint32_t ms) {
    uint32_t t1 = millis();
    while(millis() - t1 < ms) {}
}

double absVal(double d) {
    if(d < 0) return -d;
    else return d;
}

// range of at most 100 because this rng sucks
int random(int low, int high) {
    if(!seedSet) srand(millis());
    int random = rand();

    int toReturn = (random % 100) * (high - low + 1) / 99 + low;

    //  jank code to be safe and because i'm bad
    if(toReturn < low) toReturn = low;
    else if(toReturn > high) toReturn = high;

    return toReturn;
}
