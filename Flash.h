
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_FLASH_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_FLASH_H

extern uint32_t Flash_SPICounter;

extern void Flash_init();
extern uint8_t* Flash_readMemory(uint32_t address, uint32_t numBytes, uint8_t* buffer);
extern void Flash_writeMemory_specifiedAddress(uint32_t address, uint32_t numBytes, uint8_t* buffer);
extern uint32_t Flash_writeMemory(uint32_t numBytes, uint8_t* buffer);   //  writes at next available memory location
extern uint32_t Flash_allocateMemory(uint32_t bytesToAllocate);
extern uint32_t getCurrentMemoryLocation();

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_FLASH_H
