/********************************************************************
* @file       : AvgFont.h
* @author     : Glushanenko Alexander
* @version    : V1.0.0
* @date       : 28.11.2025
* @brief      : Binary font array and font rendering functions.
********************************************************************/

#include <stdint.h>

#define FONT_SIZE						1		// 1 - font 5x8; 2 - font 12x16

#if FONT_SIZE == 1
	#define FONT_USE_5x8
#elif FONT_SIZE == 2
	#define FONT_USE_12x16
#endif

#ifdef FONT_USE_5x8
#define FONT_SPACE_START		1
#define FONT_SPACE_END			1
#define FONT_X_SIZE					5
#endif

#ifdef FONT_USE_12x16
#define FONT_SPACE_START		0
#define FONT_SPACE_END			0
#define FONT_X_SIZE					12
#endif

void AvgFontPrintChar(uint8_t *buf, uint32_t bufsize, uint8_t oledwidth, uint8_t char_code, uint8_t y_pos_page, uint8_t x_pos_sym);
