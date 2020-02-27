
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

uint8_t SRAM_storage[1024*1024];     // 8 megabits of SRAM: 8 Mb / 8 bits * 1024 * 1024 = 1048576 bytes
uint32_t currentLocation = 0;

void SRAM_reset() {
    for(uint32_t i = 0; i < 1024*1024; i++) {
        SRAM_storage[i] = 0;
    }
}

void SRAM_readMemory(uint32_t location, uint16_t bytesToRead, uint8_t* buffer) {
    for(uint16_t i = 0; i < bytesToRead; i++) {
        buffer[i] = SRAM_storage[location+i];
    }
}

void SRAM_writeMemory_specifiedAddress(uint32_t location, uint32_t bytesToWrite, uint8_t* buffer) {
    for(uint16_t i = 0; i < bytesToWrite; i++) {
        SRAM_storage[location+i] = buffer[i];
    }
}

/*
 *  Writes to the next available location. Returns the starting location of data.
 */
uint32_t SRAM_writeMemory(uint32_t bytesToWrite, uint8_t* buffer) {
    uint32_t toReturn = currentLocation;
    SRAM_writeMemory_specifiedAddress(currentLocation, bytesToWrite, buffer);
    currentLocation += bytesToWrite;
    return toReturn;
}

/*
 *  Reserves space
 */
uint32_t SRAM_allocateMemory(uint32_t bytesToAllocate) {
    currentLocation += bytesToAllocate;
    return currentLocation-bytesToAllocate;
}

uint32_t getCurrentMemoryLocation() {
    return currentLocation;
}