
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

uint8_t buffer[500];
uint8_t smallBuffer2[300];

uint32_t persistentBackgroundMemLocation;
Animation animation[4][numberOfAnimations];
SpriteSendable spriteSendables[16];    // up to 16 sprites on screen at once
uint16_t activeAnimations;      // each bit represents if the corresponding sendable is being used MSB = [15], LSB = [0]
uint16_t toRemove;      // each bit represents if the corresponding sendable should be removed next update

void printArr(uint16_t size, uint8_t* buf) {
    for(uint16_t  i = 0; i < size; i++) {
        printf("%d,", buf[i]);
    }
    printf("\n");
}

void printStr(uint16_t size, uint8_t* buf) {
    for(uint16_t  i = 0; i < size; i++) {
        printf("%c,", buf[i]);
    }
    printf("\n");
}

//  returns number of bytes read
uint16_t readUntil(char delimeter, uint8_t* buf) {
    uint16_t characters = 0;
    do { buf[characters++] = SD_readNextChar(); }
    while(buf[characters-1] != delimeter);
    return characters-1;
}

//  Big endian!
uint16_t readHalfInt(uint8_t* buf) {
    return (buf[0] << 8u) + buf[1];
}

//  updates screen line by line, segment by segment using animation data
//  1.25 KB
void update() {
    for(uint8_t row = 0; row < 241; row++) {
        int16_t colorIndexes[321];  // a color index of -1 means 'do not change'
        uint8_t layer[321];

        // set all color indexes to -1 initially for 'do not change'
        memset(colorIndexes, -1, 321);

        // set all layers to background
        memset(colorIndexes, LAYER_BACKGROUND, 10);

        // Find which color index is -1
        uint8_t backgroundColorIndex = 0;
        for(uint16_t i = 0; i < sizeof(colors); i++) {
            if(colors[i] == -1) {
                backgroundColorIndex = i;
                break;
            }
        }

        //  first, loop through toRemoves and set as -1 background index (not the do not change)
        for(uint8_t slot = 0; slot < 16; slot++) {
            //  check if this slot should be removed, and continue only if it should be
            if((toRemove >> slot) & 1) {
                //  get the animation (pointer just to not make another copy)
                Animation* anim = &animation[spriteSendables[slot].charIndex][spriteSendables[slot].animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - spriteSendables->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it
                    continue;
                }

                // loop through entire width
                for(uint16_t col = 0; col < anim->width; col++) {
                    //  set the color indexes to the background color
                    colorIndexes[spriteSendables[slot].x + col] = backgroundColorIndex;
                }
            }
        }

        //  next, loop through the non-toRemoves, get their data, and set in background
        for(uint8_t slot = 0; slot < 16; slot++) {
            //  check if this slot should be removed, and continue only if it shouldn't be
            if(!((toRemove >> slot) & 1)) {
                //  get the animation (pointer just to not make another copy)
                Animation* anim = &animation[spriteSendables[slot].charIndex][spriteSendables[slot].animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - spriteSendables->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it and move on to the next sprite
                    continue;
                }

                //  TODO: fix! storage retrieval type is not correct
                uint16_t col = 0;
                while(col < anim->width) {
                    // read this row of animation into the buffer
                    uint32_t memLocation = anim->memLocation + row * spriteSendables[slot].y;
                    SRAM_readMemory(memLocation, anim->width, buffer);
                }

                for(uint16_t col = 0; col < anim->width; col++) {
                    //  write the pixel if this layer is higher than the current layer of the pixel
                    if(layer[spriteSendables[slot].x + col] < spriteSendables[slot].layer) {
                        colorIndexes[spriteSendables[slot].x + col] = buffer[col];

                        // update that layer
                        layer[spriteSendables[slot].x + col] = spriteSendables[slot].layer;
                    }
                }
            }
        }

        //  finally, draw it!

    }

    //  flag all non-persistent sprite animations to be erased next update
    for(uint8_t slot = 0; slot < 16; slot++) {
        if((activeAnimations >> slot) & 1) {
            // animation active
            if(!spriteSendables[slot].persistent) {
                toRemove |= (1u << slot);
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
            activeAnimations |= (1u << slot);
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

        //  paint row by row
        for(uint8_t row = y; row < y + animation[charIndex][animationIndex].height; row++) {
            SRAM_readMemory(animation[charIndex][animationIndex].memLocation,
                            animation[charIndex][animationIndex].width, buffer);

            uint32_t memInsertLocation = persistentBackgroundMemLocation + row * 321 + x;

            SRAM_writeMemory_specifiedAddress(memInsertLocation, animation[charIndex][animationIndex].width, buffer);
        }
    }
}

void animator_initialize() {
    persistentBackgroundMemLocation = SRAM_allocateMemory(241*321);
}

//  1 KB
void animator_readCharacterSDCard(uint8_t charIndex) {
    char filename[] = "../data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, characterNames[charIndex]);
    strcat(filename, fileType);

    SD_openFile(filename);

    //  get number of animations
    SD_read(2, buffer);
    uint16_t numAnimations = readHalfInt(buffer);

    uint8_t animationName[15];
    while(numAnimations--) {
        //  get reference index of this animation name inside metadata
        uint16_t animationIndex;
        uint8_t animationNameLength = readUntil('\n', animationName);

        //  loop through all the animation names of this character
        bool found = false;
        for (animationIndex = 0; animationIndex < numberOfAnimations; animationIndex++) {
            bool matchSoFar = true;
            //  loop through the animation index name. If there's a mismatch, move on to the next animation name
            for (uint16_t i = 0; i < animationNameLength; i++) {
                if (animationName[i] != animations[charIndex][animationIndex][i]) {
                    matchSoFar = false;
                    break;
                }
            }
            if (matchSoFar) {
                found = true;
                break;
            }
        }
        if(!found) {
            animationName[animationNameLength] = '\0'; //  prepare to print the string
            printf("ERROR Animation not found: %s\n", animationName);
            continue;
        }
        else {
            animationName[animationNameLength] = '\0'; //  prepare to print the string
            printf("Reading in animation: %s\n", animationName);
        }

        Animation *anim = &animation[charIndex][animationIndex];

        //  construct animation struct
        anim->animationIndex = animationIndex;
        anim->characterIndex = charIndex;

        SD_read(1, buffer);
        anim->frames = buffer[0];

        SD_read(2, buffer);
        anim->width = readHalfInt(buffer);

        SD_read(1, buffer);
        anim->height = buffer[0];

        printint(anim->frames);
        printint(anim->width);
        printint(anim->height);

        //  get the frame indexes, store into SRAM and smallBuffer2
        SD_read(anim->frames*3, buffer);
        anim->memLocation = SRAM_writeMemory(anim->frames*3, smallBuffer2);

        //  get the data of each frame and store it
        for (uint8_t f = 0; f < anim->frames; f++) {
            SD_read(2 * (anim->height + 1), buffer);
            SRAM_writeMemory(2 * (anim->height + 1), buffer);

            printArr(2*(anim->height + 1), buffer);

            uint32_t frameDataSize = (buffer[2 * anim->height + 1] << 8u) + buffer[2 * anim->height + 2];
            uint32_t bytesToRead = frameDataSize;

            // read and write frame data in chunks
            uint16_t maxChunkSize = 100;
            while (bytesToRead > 0) {
                uint16_t chunkSize = bytesToRead;
                if (chunkSize > maxChunkSize) chunkSize = maxChunkSize;

                SD_read(chunkSize, buffer);
                SRAM_writeMemory(chunkSize, buffer);

                bytesToRead -= chunkSize;
            }
        }
    }
}