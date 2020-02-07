
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include "SDCard.h"

using namespace std;


FILE *fp;

void SD_read(char* buffer, uint16_t bytesToRead) {
    if (fp == NULL) {
        printf("FILE ERROR");
        while (1);  // file is not open! program will stop here
    }

    char lineBuffer[10000];
    fread(buffer, 1, bytesToRead, fp);
}

char SD_readNextChar() {
    char c[1];
    fread(c, 1, 1, fp);
    return c[0];
}

void SD_openFile(char* filename) {
    fp = fopen(filename, "r");
}

void SD_closeFile() {
    fclose(fp);
}