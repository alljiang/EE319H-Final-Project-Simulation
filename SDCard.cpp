
#include <fstream>
#include <cstdint>
#include <cstdlib>
using namespace std;

#include "SDCard.h"

ifstream file;

void SD_read(char* buffer, uint8_t bytesToRead) {
    if (!file.is_open()) {
        while (1);  // file is not open! program will stop here
    }

    char b;
    for(uint32_t i = 0; i < bytesToRead && file >> b; i++) {
        buffer[i] = b;
    }
}

void SD_openFile(char* filename) {
    file.open(filename);
}

void SD_closeFile() {
    file.close();
}