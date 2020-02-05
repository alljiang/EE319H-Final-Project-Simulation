
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_SDCARD_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_SDCARD_H

extern void SD_read(uint8_t* buffer, uint8_t bytesToRead);
extern void SD_openFile(char* filename);
extern void SD_closeFile();

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_SDCARD_H
