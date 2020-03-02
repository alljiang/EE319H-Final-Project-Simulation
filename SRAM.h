
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H

extern uint32_t SRAM_SPICounter;

extern void SRAM_reset();
extern void SRAM_readMemory(uint32_t location, uint16_t bytesToRead, uint8_t* buffer);
extern void SRAM_writeMemory_specifiedAddress(uint32_t location, uint32_t bytesToWrite, uint8_t* buffer);
extern uint32_t SRAM_writeMemory(uint32_t bytesToWrite, uint8_t* buffer);
extern uint32_t SRAM_allocateMemory(uint32_t bytesToAllocate);
extern uint32_t getCurrentMemoryLocation();

#endif // EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H
