
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include "SD.h"

using namespace std;


FILE *fp;

void SD_read(uint32_t bytesToRead, uint8_t* buffer) {
    if (fp == NULL) {
        printf("FILE ERROR\n");
        return;
    }
    for(int i = 0; i < bytesToRead; i++) {
        fread(buffer + i, 1, 1, fp);
    }
}

char SD_readNextChar() {
    char c[1];
    fread(c, 1, 1, fp);
    return c[0];
}

void SD_openFile(char* filename) {
    fp = fopen(filename, "rb");
}

void SD_closeFile() {
    if(fp != NULL)
        fclose(fp);
}