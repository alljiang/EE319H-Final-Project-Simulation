
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "animator.h"
#include "metadata.h"
#include "SRAM.h"
#include "SDCard.h"
#include "utils.h"
#include <cmath>

Animation animation[4][50];

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

    char buffer[100];
    uint16_t chars;

    //  get number of animations
    uint8_t numAnimations = readNextNumber('\n', buffer);

    while(numAnimations--) {
        chars = readUntil('\n', buffer);

        //  get index of this animation
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

        //  construct animation struct
        animation[charIndex][animationIndex].animationIndex = animationIndex;
        animation[charIndex][animationIndex].characterIndex = charIndex;
        animation[charIndex][animationIndex].frames = readNextNumber('\n', buffer);
        printint(animation[charIndex][animationIndex].frames);
        animation[charIndex][animationIndex].width =  readNextNumber('\n', buffer);
        printint(animation[charIndex][animationIndex].width);
        animation[charIndex][animationIndex].height =  readNextNumber('\n', buffer);
        printint(animation[charIndex][animationIndex].height);
        println("");

        uint8_t buffer[animation[chars][animationIndex].width];
        for(uint8_t f = 0; f < animation[charIndex][animationIndex].frames; f++) {
            for (uint8_t r = 0; r < animation[charIndex][animationIndex].height; r++) {
                for (uint8_t c = 0; c < animation[chars][animationIndex].width; c += 2) {
                    buffer[
                    f * animation[charIndex][animationIndex].height * animation[charIndex][animationIndex].width
                    + r * animation[charIndex][animationIndex].width + c
                    ] = readNextNumber(' ', (char*)buffer);
//                    SRAM_writeMemory()
                }
            }
        }

        break;
    }




}