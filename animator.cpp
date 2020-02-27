
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
#include <cmath>
using namespace std;

#define maxSprites 16

uint8_t buffer[2000];   // it's big because i can. if this is too big, lower it down to like 700
uint8_t smallBuffer2[300];
int16_t colorIndexes[321];  // a color index of -1 means 'do not change'
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
    for(uint8_t row = 0; row <= 241; row++) {
        // set all color indexes to -2 initially for 'do not change'
        // set all layers to background
        for(uint16_t col = 0; col <= 321; col++) {
            colorIndexes[col] = COLOR_DONOTCHANGE;
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
                    colorIndexes[spriteSendables[slot].x + col] = COLOR_BACKGROUND;
                }
            }
        }

        //  next, loop through the active non-toRemoves and get their data
        for(uint8_t slot = 0; slot < 16; slot++) {
            //  check if this slot should be removed, and continue only if it shouldn't be
            if( ( !((toRemove >> slot) & 1) ) && ( (activeAnimations >> slot) & 1 ) ) {
                //  get the animation (pointer just to not make another copy)
                SpriteSendable* ss = &spriteSendables[slot];
                Animation* anim = &animation[ss->charIndex][ss->animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - ss->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it and move on to the next sprite
                    continue;
                }

                //  TODO: fix! storage retrieval type is not correct
                //  This row intersects! Now, paint this row of this animation into the color index buffer

                //  get frame location with frame index array
                SRAM_readMemory(anim->memLocation + ss->frame, 3, buffer);
                uint32_t frameLocation = anim->memLocation + (buffer[0] << 8u) + buffer[1];

                //  get row location with row index array
                SRAM_readMemory(frameLocation + heightDifference, 4, buffer);
                uint32_t rowStart = (buffer[0] << 8u) + buffer[1];
                uint32_t rowSize = (buffer[2] << 8u) + buffer[3] - rowStart;

                //  read the row of data into the buffer
                SRAM_readMemory(rowStart, rowSize, buffer);

                //  copy the data over from the buffer into the color index buffer
                for(uint16_t col = 0; col < anim->width; col++) {
                    if(ss->layer <= layer[ss->x + col]) continue;   //  this sprite has lower layer priority

                    uint16_t colorIndex = (buffer[col*4+0] << 8u) + (buffer[col*4+1]);
                    if(colorIndex == backgroundColorIndex) continue;    //  this is the background color, ignore it

                    uint16_t quantity = (buffer[col*4+2] << 8u) + (buffer[col*4+3]);

                    //  add the colorIndex to colorIndexes quantity times, update the layer there too
                    while(quantity-- > 0) {
                        colorIndexes[ss->x + col] = colorIndex;
                        layer[ss->x + col] = ss->layer;
                        col++;
                    }
                }
            }
        }

        //  next, replace all the -1s with the background from SRAM background reserve
        for(uint16_t col = 0; col <= 321; col++) {
            if(colorIndexes[col] != COLOR_BACKGROUND) continue;

            //  if there are -1s in a row, read them together to save time
            uint16_t consecutiveBackgroundRowSize = 1;
            while((col + consecutiveBackgroundRowSize) < 320) {
                if(colorIndexes[col + consecutiveBackgroundRowSize + 1] == COLOR_BACKGROUND) {
                    consecutiveBackgroundRowSize++;
                }
                else break;
            }

            //  read in from SRAM
            uint32_t backgroundRowLocation = persistentBackgroundMemLocation + row * 321 * 2 + col * 2;
            SRAM_readMemory(backgroundRowLocation, consecutiveBackgroundRowSize, buffer);

            //  copy from buffer into colorIndexes array
            for(uint16_t i = 0; i < consecutiveBackgroundRowSize; i++) {
                colorIndexes[col + i] = (buffer[i*2 + 0] << 8u) + (buffer[i*2 + 1]);
            }

            col += consecutiveBackgroundRowSize-1;
        }

        //  finally, draw it!

        //  send in segments divided by "Do not change" colors (-2s)
        for(uint16_t col = 0; col <= 321; col++) {
            if(colorIndexes[col] == COLOR_DONOTCHANGE) continue;

            //  find length of segment
            uint16_t consecutiveColorSize = 1;
            while((col + consecutiveColorSize) < 320) {
                if(colorIndexes[col + consecutiveColorSize + 1] != COLOR_DONOTCHANGE) {
                    consecutiveColorSize++;
                }
                else break;
            }

            //  write this section into the LCD
            ILI9341_drawColors_indexed(col, row, colorIndexes+col, consecutiveColorSize);
        }
    }

    //  flag all non-persistent sprite animations to move to next frame or be erased next update
    for(uint8_t slot = 0; slot < 16; slot++) {
        if((activeAnimations >> slot) & 1) {
            SpriteSendable* ss = &spriteSendables[slot];
            Animation* anim = &animation[ss->charIndex][ss->animationIndex];

            // animation active
            if(!ss->persistent) {
                if(++ss->frame > anim->frames)
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
    persistentBackgroundMemLocation = SRAM_allocateMemory(241*321*2);
}

//  1 KB
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
        SD_read(anim->frames*3, buffer);
        anim->memLocation = SRAM_writeMemory(anim->frames*3, smallBuffer2);

        //  get the data of each frame and store it
        for (uint8_t f = 0; f < anim->frames; f++) {
            SD_read(2*(anim->height+1), buffer);
            SRAM_writeMemory(2 * (anim->height + 1), buffer);

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