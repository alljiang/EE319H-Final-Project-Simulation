//
// Created by Allen on 2/4/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H

/*
 *
 */
extern uint8_t* SRAM_readMemory(uint32_t location, uint32_t bytesToRead);
void SRAM_writeMemory(uint8_t* byteArray, uint16_t location, uint16_t bytesToWrite);

#endif // EE319K_FINAL_PROJECT_INITIAL_TESTING_SRAM_H
