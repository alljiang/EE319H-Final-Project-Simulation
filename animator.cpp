
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "animator.h"
#include "metadata.h"
#include "SRAM.h"
#include "SDCard.h"

uint16_t animationIndex[numberOfAnimations];
uint16_t animationHeight[numberOfAnimations];

void animator_readAnimations(char* name) {
    char filename[] = "../data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, name);
    strcat(filename, fileType);

    SD_openFile(filename);

    char buffer[100];
    SD_read(buffer, 10);

    printf(buffer);
    printf("asdf%d", buffer[2] == '\n');
}