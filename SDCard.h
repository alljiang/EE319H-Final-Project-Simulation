
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_SDCARD_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_SDCARD_H

extern void SD_read(uint32_t bytesToRead, uint8_t* buffer);
extern char SD_readNextChar();
extern void SD_openFile(char* filename);
extern void SD_closeFile();

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_SDCARD_H