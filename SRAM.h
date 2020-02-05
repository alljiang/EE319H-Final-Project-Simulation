
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H

extern void SRAM_readMemory(uint8_t* buffer, uint16_t location, uint16_t bytesToRead);
extern void SRAM_writeMemory(uint8_t* buffer, uint16_t location, uint16_t bytesToWrite);

#endif // EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H
