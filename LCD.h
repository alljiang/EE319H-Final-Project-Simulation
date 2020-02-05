
#include <cstdlib>
#include <cstdint>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H

extern void LCD_startLCD();
extern void LCD_drawPixel(uint16_t x, uint16_t y, uint32_t rgb);
extern void LCD_update();
extern void stopSDL2();
extern void updateWindowTitle();
extern void print(uint8_t *str);

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H
