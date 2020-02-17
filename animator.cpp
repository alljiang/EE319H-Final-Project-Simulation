
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "animator.h"
#include "metadata.h"
#include "SRAM.h"
#include "SDCard.h"
#include "utils.h"
#include <cmath>
using namespace std;

uint32_t persistentBackgroundMemLocation;
Animation animation[4][numberOfAnimations];
SpriteSendable spriteSendables[16];    // up to 16 sprites on screen at once
uint16_t activeAnimations;      // each bit represents if the corresponding sendable is being used MSB = [15], LSB = [0]
uint16_t toRemove;      // each bit represents if the corresponding sendable should be removed next update

void prBuf(uint16_t endIndex, char* buffer) {
    for(uint16_t  i = 0; i < endIndex; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");
}

//  returns number of bytes read
char readUntil(char delimeter, char* buffer) {
    uint16_t characters = 0;
    do { buffer[characters++] = SD_readNextChar(); }
    while(buffer[characters-1] != delimeter);
    return characters-1;
}

uint32_t readNextNumber(char delimeter, char* buffer) {
    //  get number of animations
    uint8_t number = 0;
    uint8_t chars = readUntil(delimeter, buffer);
    for(uint8_t i = 0; i < chars; i++) number += pow(10, chars - i - 1) * (buffer[i] - '0');
    return number;
}

//  updates screen line by line, segment by segment using animation data
//  1.25 KB
void update() {
    for(uint8_t row = 0; row < 241; row++) {
        uint8_t colorIndexes[321];
        uint8_t layer[321];
        bool changed[321];
        uint8_t buffer[321];

        for(uint8_t slot = 0; slot < 16; slot++) {
            //  loop through toRemoves and set as -1s in background
        }

//        for(uint8_t slot = 0; slot < 16; slot++) {
//            if(!((activeAnimations >> slot) & 1))   //  skip if not using animation
//                continue;
//
//            SpriteSendable* ss = &spriteSendables[slot];
//            Animation* a = &animation[ss->charIndex][ss->animationIndex];
///*
// *          put all sprites on row if sprite is inside row (can affect row)
// */
//            if(row >= ss->y && row < ss->y + a->height) {       //  if sprite affects current row
//
//                uint32_t memLocation = a->memLocation + ss->frame * a->width * a->height + (a->height - 1 - (row - ss->y));
//                SRAM_readMemory(memLocation, a->width, buffer); //  read sprite data into buffer (contains excess)
//
//                uint16_t currentX = ss->x;
//                uint16_t arrayTrasversal = 0;
//                for(uint16_t pixel = 0; pixel < a->width; pixel++) {
//                    uint8_t repetitions = buffer[arrayTrasversal++];
//                    uint8_t colorIndex = buffer[arrayTrasversal++];
//                    while(repetitions-- && pixel < a->width) {
//                        colorIndexes[pixel + currentX] = colorIndex;
//                        if(layer[pixel] < ss->layer) {
//                            layer[pixel] = ss->layer;
//                            changed[pixel] = true;
//                        }
//                        pixel++;
//                    }
//                }
//            }
//        }
        for(uint8_t slot = 0; slot < 16; slot++) {
            SpriteSendable* ss = &spriteSendables[slot];
            Animation* a = &animation[ss->charIndex][ss->animationIndex];

            //  erase sprites that are to be removed
            if((toRemove >> slot) & 1) {
                //  Read the background of the sprite that is to be removed
                SRAM_readMemory(persistentBackgroundMemLocation + 321 * row + ss->x, a->width, buffer);

                //  Set only the spots that are unchanged and need to be removed
                for(uint16_t c = 0; c < a->width; c++) {
                    if(!changed[c + ss->x]) {
                        colorIndexes[c + ss->x] = buffer[c];
                        changed[c + ss->x] = true;
                    }
                }

            }
        }
    }

    //  flag all non-persistent sprite animations to be erased next update
    for(uint8_t slot = 0; slot < 16; slot++) {
        if((activeAnimations >> slot) & 1) {
            // animation active
            if(!spriteSendables[slot].persistent) {
                toRemove |= (1 << slot);
            }
        }
    }
}

//  receive from UART, adds an animation to be displayed
void animator_animate(uint8_t charIndex, uint8_t animationIndex,
        uint8_t x, uint16_t y, uint8_t frame, uint8_t layer, uint8_t persistent) {

    //  find first unused animation slot
    uint8_t slot;
    for(slot = 0; slot < 16; slot++) {
        bool inUse = (activeAnimations >> slot) & 1;
        if(!inUse) {
            //  we about to start using it, so flag it as in use
            activeAnimations |= (1 << slot);
            break;
        }
    }

    spriteSendables[slot].charIndex = charIndex;
    spriteSendables[slot].animationIndex = animationIndex;
    spriteSendables[slot].x = x;
    spriteSendables[slot].y = y;
    spriteSendables[slot].frame = frame;
    spriteSendables[slot].layer = layer;
    spriteSendables[slot].persistent = persistent;

    if(spriteSendables[slot].persistent) {
        //  add to persistent array in SRAM

        uint8_t buffer[321];
        //  paint row by row
        for(uint8_t row = y; row < y + animation[charIndex][animationIndex].height; row++) {
            SRAM_readMemory(animation[charIndex][animationIndex].memLocation,
                            animation[charIndex][animationIndex].width, buffer);

            uint32_t memInsertLocation = persistentBackgroundMemLocation + row * 321 + x;

            SRAM_writeMemory_specifiedAddress(memInsertLocation, animation[charIndex][animationIndex].width, buffer);
        }
    }
}

//  1 KB
void animator_readCharacterSDCard(uint8_t charIndex) {
    char filename[] = "../data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, characterNames[charIndex]);
    strcat(filename, fileType);

    SD_openFile(filename);

    char buffer[10];
    uint16_t chars;

    //  get number of animations
    uint8_t numAnimations = readNextNumber('\n', buffer);

    while(numAnimations--) {
        chars = readUntil('\n', buffer);

        //  get reference index of this animation name inside metadata
        uint16_t animationIndex;
        for(animationIndex = 0;; animationIndex++) {
            bool found = true;
            for(uint16_t i = 0; i < chars; i++) {
                if(buffer[i] != animations[charIndex][animationIndex][i]) {
                    found = false;
                    break;
                }
            }
            if(found) break;
        }

        Animation* anim = &animation[charIndex][animationIndex];

        //  construct animation struct
        anim->animationIndex = animationIndex;
        anim->characterIndex = charIndex;
        anim->frames = readNextNumber('\n', buffer);
        anim->width =  readNextNumber('\n', buffer);
        anim->height =  readNextNumber('\n', buffer);
        println("");


        uint8_t lineBuffer[1000];
        anim->memLocation = getCurrentMemoryLocation();
        for(uint8_t f = 0; f < anim->frames; f++) {
            for (uint8_t r = 0; r < anim->height; r++) {
                uint16_t pairCount = 0;
                uint16_t pixels = anim->width;
                for (uint8_t c = 0; pixels > 0; c++) {
                    lineBuffer[2*c] = readNextNumber(' ', (char*)buffer);   //  color index
                    lineBuffer[2*c + 1] = readNextNumber(' ', (char*)buffer);   //  number of pixels
                    pixels -= lineBuffer[2*c + 1];
                    pairCount++;
                }

                SRAM_writeMemory_specifiedAddress(
                        anim->memLocation + f * (anim->height * anim->width) + r * anim->width,
                        pairCount, lineBuffer);
            }
        }
        readUntil('\n', buffer);    //  clear the new line
    }
}

void animator_initialize() {
    persistentBackgroundMemLocation = SRAM_allocateMemory(241*321);
}