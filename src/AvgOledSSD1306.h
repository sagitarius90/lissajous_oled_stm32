/********************************************************************
* @file       : AvgOledSSD1306.h
* @author     : Glushanenko Alexander
* @version    : V1.0.0
* @date       : 28.11.2025
* @brief      : OLED SSD1306 display initialization and rendering.
********************************************************************/

#include <stdint.h>

#define OLED_WIDTH            128
#define OLED_HEIGHT           64  // <<< %8 == 0  !!!
#define OLED_BUFFER_SIZE			OLED_WIDTH * OLED_HEIGHT/8
#define OLED_NUMBER_DIVIDER   '.'

#define OLED_I2C_ADDRESS      0x3C
#define OLED_I2C_COMMAND      0
#define OLED_I2C_DATA         0x40

void AvgOledInit(uint8_t include_i2c_init, uint8_t inverse,	uint8_t contrast, uint8_t precharge);
void AvgOledUpdate(void);
void AvgOledClear(void);
void AvgOledClearRect(uint8_t x_first, uint8_t x_last, uint8_t page_first, uint8_t page_last);
void AvgOledWriteBuffer(uint8_t *bitmap, uint32_t bitmapSize);
void AvgOledSetXY(uint8_t pixelState, uint8_t x, uint8_t y);
void AvgOledPrintChar(uint8_t char_code, uint8_t y_pos_page, uint8_t x_pos_sym);
void AvgOledPrintString(uint8_t *str, uint8_t length, uint8_t y_pos_str, uint8_t x_pos_sym);
