
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Flash.h"

/*
 *
 * Memory simulation:
 *
 * EVERYTHING is stored inside a one-dimensional array, which basically simulates the Flash chip.
 *
 * To read memory, simply request the location and the number of bytes to read starting from that location
 *
 * Make sure to store the starting location and size of everything you send to the Flash chip.
 *
 * After reading an array from the Flash memory, free up that space once you're done with "delete <variable name>;"
 */

uint8_t Flash_storage[1024*1024];     // 8 megabits of Flash: 8 Mb / 8 bits * 1024 * 1024 = 1048576 bytes
uint32_t currentLocation = 0;

uint32_t Flash_SPICounter = 0;

void Flash_init() {
    for(uint32_t i = 0; i < 1024*1024; i++) {
        Flash_storage[i] = 0;
    }
    currentLocation = 0;
}

uint8_t* Flash_readMemory(uint32_t location, uint32_t bytesToRead, uint8_t* buffer) {
    for(uint16_t i = 0; i < bytesToRead; i++) {
        buffer[i] = Flash_storage[location+i];
    }
    Flash_SPICounter += 4 + bytesToRead;
    return buffer;
}

void Flash_writeMemory_specifiedAddress(uint32_t location, uint32_t bytesToWrite, uint8_t* buffer) {
    for(uint16_t i = 0; i < bytesToWrite; i++) {
        Flash_storage[location+i] = buffer[i];
    }
}

/*
 *  Writes to the next available location. Returns the starting location of data.
 */
uint32_t Flash_writeMemory(uint32_t bytesToWrite, uint8_t* buffer) {
    uint32_t toReturn = currentLocation;
    Flash_writeMemory_specifiedAddress(currentLocation, bytesToWrite, buffer);
    currentLocation += bytesToWrite;
    return toReturn;
}

/*
 *  Reserves space
 */
uint32_t Flash_allocateMemory(uint32_t bytesToAllocate) {
    currentLocation += bytesToAllocate;
    return currentLocation-bytesToAllocate;
}

uint32_t getCurrentMemoryLocation() {
    return currentLocation;
}