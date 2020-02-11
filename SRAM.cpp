
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

uint8_t SRAM_storage[8*1024*1024];     // 8 megabytes of SRAM: 8 * 1024 * 1024 = 8388608 bytes
uint32_t currentLocation = 0;

void SRAM_readMemory(uint32_t location, uint16_t bytesToRead, uint8_t* buffer) {
    for(uint16_t i = 0; i < bytesToRead; i++) {
        buffer[i] = SRAM_storage[location+i];
    }
}

void SRAM_writeMemory_specifiedAddress(uint32_t location, uint16_t bytesToWrite, uint8_t* buffer) {
    for(uint16_t i = 0; i < bytesToWrite; i++) {
        SRAM_storage[location+i] = buffer[i];
    }
}

/*
 *  Writes to the next available location. Returns the starting location of data.
 */
uint32_t SRAM_writeMemory(uint16_t bytesToWrite, uint8_t* buffer) {
    SRAM_writeMemory_specifiedAddress(currentLocation, bytesToWrite, buffer);
    currentLocation += bytesToWrite;
    return (currentLocation - bytesToWrite);
}

uint32_t getCurrentMemoryLocation() {
    return currentLocation;
}