//
// Created by Allen on 2/4/2020.
//

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "SRAM.h"

/*
 *
 * Memory simulation:
 *
 * EVERYTHING is stored inside a one-dimensional array, which basically simulates the SRAM chip.
 *
 * To read memory, simply request the location and the number of bytes to read starting from that location
 *
 * Make sure to store the starting location and size of everything you send to the SRAM chip.
 *
 * After reading an array from the SRAM memory, free up that space once you're done with "delete <variable name>;"
 */

uint8_t SRAM_storage[8388608];     // 8 megabytes of SRAM: 8 * 1024 * 1024 = 8388608 bytes

uint8_t* SRAM_readMemory(uint16_t location, uint16_t bytesToRead) {
    uint8_t* newList = new uint8_t[bytesToRead];
    for(uint16_t i = 0; i < bytesToRead; i++) {
        newList[i] = SRAM_storage[location+i];
    }
    return newList;
}

void SRAM_writeMemory(uint8_t* byteArray, uint16_t location, uint16_t bytesToWrite) {
    for(uint16_t i = 0; i < bytesToWrite; i++) {
        SRAM_storage[location+i] = byteArray[i];
    }
}