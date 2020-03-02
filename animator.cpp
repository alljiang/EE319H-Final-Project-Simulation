
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "animator.h"
#include "metadata.h"
#include "SRAM.h"
#include "SDCard.h"
#include "utils.h"
#include "LCD.h"
#include "colors.h"
#include <cmath>
using namespace std;

#define maxSprites 16

uint8_t buffer[1000];   // it's big because i can. if this is too big, lower it down to like 700
uint8_t smallBuffer2[300];
int32_t colorIndexes[321];  // a color index of -1 means 'do not change'
uint8_t layer[321];

uint32_t persistentBackgroundMemLocation;
Animation animation[4][numberOfAnimations];
SpriteSendable spriteSendables[maxSprites]; // up to maxSprites sprites on screen at once
uint16_t activeAnimations = 0;  // each bit represents if the corresponding sendable is being used, big endian
uint16_t toRemove = 0;  // each bit represents if the corresponding sendable should be removed next update, big endian

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
void animator_update() {
    for(uint8_t row = 0; row <= 240; row++) {
        // set all color indexes to -2 initially for 'do not change'
        // set all layers to background
        for(uint16_t col = 0; col <= 320; col++) {
            colorIndexes[col] = ANIMATOR_COLOR_DONOTCHANGE;
            layer[col] = LAYER_BACKGROUND;
        }


        // Find which color index is 0xFFFFFFFF (background)
        uint16_t backgroundColorIndex = 0;
        for(uint16_t i = 0; i < sizeof(colors); i++) {
            if(colors[i] == 0xFFFFFFFF) {
                backgroundColorIndex = i;
                break;
            }
        }

        //  first, loop through toRemoves and set as -1 background index (need to change)
        for(uint8_t slot = 0; slot < maxSprites; slot++) {
            //  check if this slot should be removed, and continue only if it should be
            if((toRemove >> slot) & 1) {
                //  get the animation (pointer just to not make another copy)
                SpriteSendable* ss = &spriteSendables[slot];
                Animation* anim = &animation[ss->charIndex][ss->animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - ss->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it
                    continue;
                }

                // loop through entire width
                for(uint16_t col = 0; col < anim->width; col++) {
                    //  set the color indexes to the background color
                    if(ss->mirrored) {
                        if(ss->x+(anim->width) - col > 320 || ss->x+(anim->width) - col < 0) continue;
                        colorIndexes[ss->x+(anim->width) - col] = ANIMATOR_COLOR_BACKGROUND;
                    }
                    else {
                        if(ss->x + col > 320 || ss->x + col < 0) continue;
                        colorIndexes[ss->x + col] = ANIMATOR_COLOR_BACKGROUND;
                    }
                }
            }
        }

        //  next, loop through the actives and get their data
        for(uint8_t slot = 0; slot < 16; slot++) {
            //  check if this slot should be removed, and continue only if it shouldn't be
            if( ( (activeAnimations >> slot) & 1u ) ) {
                //  get the animation (pointer just to not make another copy)
                SpriteSendable* ss = &spriteSendables[slot];
                Animation* anim = &animation[ss->charIndex][ss->animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - ss->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it and move on to the next sprite
                    continue;
                }

                //  This row intersects! Now, paint this row of this animation into the color index buffer

                //  get frame location with frame index array
                SRAM_readMemory(anim->memLocation + 3*ss->frame, 3, buffer);
                uint32_t frameLocation = anim->memLocation  //  start location
                        + anim->frames*3    //  frame index array
                        + (anim->height+1)*2 * ss->frame    //  row index array
                        + ((buffer[0] << 16u) + (buffer[1] << 8u) + buffer[2])*2;    //  frame location offset

                //  get row location with row index array
                SRAM_readMemory(frameLocation + (anim->height - heightDifference - 1)*2, 4, buffer);
                uint32_t rowStartOffset = (buffer[0] << 8u) + buffer[1];
                uint32_t rowSize = ((buffer[2] << 8u) + buffer[3] - rowStartOffset);

                uint32_t rowStartLocation = frameLocation + (anim->height+1)*2 + rowStartOffset;

                //  read the row of data into the buffer
                SRAM_readMemory(rowStartLocation, rowSize, buffer);

                //  copy the data over from the buffer into the color index buffer
                uint16_t column = 0;
                uint16_t numPairs = rowSize >> 2u;
                for(uint16_t pair = 0; pair < numPairs; pair++) {
//                    if(ss->mirrored) {
//                        if(ss->x + (anim->width) - column > 320 || ss->x + (anim->width) - column < 0) continue;
//                        if (ss->layer <= layer[ss->x + (anim->width) - column])
//                            continue;   //  this sprite has lower layer priority
//                    }
//                    else {
//                        if(ss->x + column > 320 || ss->x + column < 0) continue;
//                        if (ss->layer <= layer[ss->x + column]) continue;   //  this sprite has lower layer priority
//                    }

                    uint16_t colorIndex = (buffer[pair*4+0] << 8u) + (buffer[pair*4+1]);
                    uint16_t quantity = (buffer[pair*4+2] << 8u) + (buffer[pair*4+3]);

                    if(colorIndex == backgroundColorIndex) {
                        //  this is the background color, ignore it
                        column += quantity;
                        continue;
                    }

                    //  add the colorIndex to colorIndexes quantity times, update the layer there too
                    while(quantity-- > 0) {
                        if(ss->mirrored) {
                            if(!(ss->x+(anim->width) - column > 320 || ss->x+(anim->width) - column < 0)) {
                                if (layer[ss->x + (anim->width) - column] < ss->layer) {
                                    colorIndexes[ss->x + (anim->width) - column] = colorIndex;
                                    layer[ss->x + (anim->width) - column] = ss->layer;
                                }
                            }
                        }
                        else {
                            if(!(ss->x + column > 320 || ss->x + column < 0)) {
                                if (layer[ss->x + column] < ss->layer) {
                                    colorIndexes[ss->x + column] = colorIndex;
                                    layer[ss->x + column] = ss->layer;
                                }
                            }
                        }
                        column++;
                    }
                }
            }
        }

        //  next, replace all the -1s with the background from SRAM background reserve
        for(uint16_t col = 0; col <= 320; col++) {
            if(colorIndexes[col] != ANIMATOR_COLOR_BACKGROUND) continue;

            //  if there are -1s in a row, read them together to save time
            uint16_t consecutiveBackgroundRowSize = 1;
            while((col + consecutiveBackgroundRowSize) < 320) {
                if(col + consecutiveBackgroundRowSize > 320) continue;
                if(colorIndexes[col + consecutiveBackgroundRowSize] == ANIMATOR_COLOR_BACKGROUND) {
                    consecutiveBackgroundRowSize++;
                }
                else break;
            }

            //  read in from SRAM
            uint32_t backgroundRowLocation = persistentBackgroundMemLocation + row * 321 * 2 + col * 2;
            SRAM_readMemory(backgroundRowLocation, 2*consecutiveBackgroundRowSize, buffer);

            //  copy from buffer into colorIndexes array
            for(uint16_t i = 0; i < consecutiveBackgroundRowSize; i++) {
                if(col+i > 320 || col + i < 0) continue;
                colorIndexes[col + i] = (buffer[i*2 + 0] << 8u) + (buffer[i*2 + 1]);
            }

            col += consecutiveBackgroundRowSize-1;
        }

        //  finally, draw it!

        //  send in segments divided by "Do not change" colors (-2s)
        for(uint16_t col = 0; col <= 320; col++) {
            if(colorIndexes[col] == ANIMATOR_COLOR_DONOTCHANGE) continue;

            //  find length of segment
            uint16_t consecutiveSize = 1;
            while((col + consecutiveSize) < 320) {
                if(colorIndexes[col + consecutiveSize] != ANIMATOR_COLOR_DONOTCHANGE) {
                    consecutiveSize++;
                }
                else break;
            }

            //  write this section into the LCD
            ILI9341_drawColors_indexed(col, row, colorIndexes+col, consecutiveSize);

            col += consecutiveSize -1;
        }
    }

    //  clear all toRemove flags
    toRemove = 0;

    //  flag all non-persistent sprite animations to move to next frame and be erased next update
    for(uint8_t slot = 0; slot < 16; slot++) {
        if((activeAnimations >> slot) & 1) {
            SpriteSendable* ss = &spriteSendables[slot];
            Animation* anim = &animation[ss->charIndex][ss->animationIndex];

            // animation active
            if(!ss->persistent) {
                //  if this is supposed to last longer, let it last longer
                if(ss->framePeriod - ++ss->currentframeLength > 0) continue;
                else ss->currentframeLength = 0;

                ss->frame += 1;
                if((ss->frame >= anim->frames) || !ss->continuous) {
                    activeAnimations &= ~(1u << slot);
                }
                toRemove |= (1u << slot);
            }
        }
    }
}

//  receive from UART, adds an animation to be displayed
void animator_animate(uint8_t charIndex, uint8_t animationIndex,
        int16_t x, int16_t y, uint8_t frame, uint8_t animationlayer, uint8_t persistent,
        uint8_t continuous, uint8_t framePeriod, bool mirrored) {

    //  find first unused animation slot
    uint8_t slot;
    for(slot = 0; slot < 16; slot++) {
        bool inUse = (activeAnimations >> slot) & 1 || (toRemove >> slot) & 1;
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
    spriteSendables[slot].layer = animationlayer;
    spriteSendables[slot].persistent = persistent;
    spriteSendables[slot].continuous = continuous;
    spriteSendables[slot].framePeriod = framePeriod;
    spriteSendables[slot].currentframeLength = 0;
    spriteSendables[slot].mirrored = mirrored;

//    if(mirrored) spriteSendables[slot].x

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
    persistentBackgroundMemLocation = SRAM_allocateMemory(241*321*2);
}

void animator_readPersistentSprite(const char* spriteName, uint16_t x, uint8_t y) {
    char filename[] = "../data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, spriteName);
    strcat(filename, fileType);

    SD_closeFile();
    SD_openFile(filename);

    printf("Reading in sprite: %s\n", spriteName);


    SD_read(2, buffer);
    uint16_t width = readHalfInt(buffer);

    SD_read(1, buffer);
    uint8_t height = buffer[0];

    //  get the data and store it
    for(int16_t row = height-1; row >= 0; row--) {
        SD_read(width*2, buffer);

        uint32_t SRAMRowLocation = persistentBackgroundMemLocation + (row-y) * 321*2 + x;
        SRAM_writeMemory_specifiedAddress(SRAMRowLocation, width*2, buffer);

        //  change colors to 16 bit array to process in a sec
        for (uint16_t i = 0; i < 321; i++) {
            colorIndexes[i] = (buffer[2*i]<<8u) + buffer[2*i+1];
        }

        ILI9341_drawColors_indexed(x, row, colorIndexes, width);
    }
}

void animator_readCharacterSDCard(uint8_t charIndex) {
    char filename[] = "../data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, characterNames[charIndex]);
    strcat(filename, fileType);

    SD_closeFile();
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

        //  get the frame indexes, store into SRAM and smallBuffer2
        SD_read(anim->frames*3, smallBuffer2);
        anim->memLocation = SRAM_writeMemory(anim->frames*3, smallBuffer2);

        //  get the data of each frame and store it
        for (uint8_t f = 0; f < anim->frames; f++) {
            SD_read(2*(anim->height+1), buffer);
            uint32_t x = SRAM_writeMemory(2 * (anim->height + 1), buffer);
            uint32_t frameDataSize = (buffer[2 * anim->height] << 8u) + buffer[2 * anim->height + 1];
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