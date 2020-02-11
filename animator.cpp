
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "animator.h"
#include "metadata.h"
#include "SRAM.h"
#include "SDCard.h"
#include "utils.h"
#include <cmath>

Animation animation[4][numberOfAnimations];

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


        uint8_t lineBuffer[anim->width*2];
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
                printf("%d, %d, %d, %d, %d, %d, %d, %d", lineBuffer[0], lineBuffer[1], lineBuffer[2],
                        lineBuffer[3], lineBuffer[4], lineBuffer[5], lineBuffer[6], lineBuffer[7]);
                println("");
                SRAM_writeMemory_specifiedAddress(
                        anim->memLocation + f * (anim->height * anim->width) + r * anim->width,
                        2*pairCount, lineBuffer);
            }
        }
        readUntil('\n', buffer);    //  clear the new line
    }

}