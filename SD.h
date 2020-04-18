
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_SD_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_SD_H

extern void SD_read(uint32_t bytesToRead, uint8_t* buffer);
extern char SD_readNextChar();
extern void SD_openFile(char* filename);
extern void SD_closeFile();
extern void SD_startSDCard();

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_SD_H
