
#include <cstdlib>
#include <cstdint>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H

extern void LCD_startLCD();
extern void ILI9341_drawColors_indexed(uint32_t x, uint32_t y, int16_t *rgbIndex, uint16_t totalPixels);
extern void LCD_drawPixel(uint16_t x, uint16_t y, uint32_t rgb);
extern void LCD_update();
extern void stopSDL2();

/*
 *  @brief       Draws a multi-colored line, optimized for efficiency
 *
 *  @param      x       x-coordinate of left-most pixel
 *  @param      y       y-coordinate of line
 *  @param      *rgb    pointer to array of RGB values (parallel to num, must be same array length)
 *  @param      *num    pointer to array of number of pixels corresponding to each RGB color
 *  @param      n       size of array
 */
extern void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n);

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H
