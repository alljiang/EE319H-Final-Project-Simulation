
#include <cstdlib>
#include <cstdint>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H

extern uint32_t ILI9341_SPICounter;

extern void LCD_startLCD();
extern void ILI9341_drawColors_indexed(uint32_t x, uint32_t y, int32_t *rgbIndex, uint16_t totalPixels);
extern void ILI9341_fillScreen(uint32_t rgb);
extern void LCD_drawOverlayCircle(double doublex, double doubley, uint8_t radius, uint32_t color);
extern void LCD_drawOverlayRectangle(double doublex, double doubley, uint8_t width, uint8_t height, uint32_t colorSub);
extern void LCD_clearOverlay();
extern void LCD_drawPixel(uint16_t x, uint16_t y, uint32_t rgb, bool isOverlay);
extern void LCD_update();
extern void stopSDL2();

/*
 *  @brief       Draws a multi-colored line, optimized for efficiency
 *
 *  @param      x       x-coordinate of left-most pixel
 *  @param      y       y-coordinate of line
 *  @param      *rgb    pointer to array of RGB values (parallel to num, must be same array height)
 *  @param      *num    pointer to array of number of pixels corresponding to each RGB color
 *  @param      n       size of array
 */
extern void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n);

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_LCD_H
