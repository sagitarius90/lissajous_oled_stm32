/********************************************************************
* @file       : AvgOledSSD1306.c
* @author     : Glushanenko Alexander
* @version    : V1.0.0
* @date       : 28.11.2025
* @brief      : OLED SSD1306 display initialization and rendering.
********************************************************************/

#include "AvgOledSSD1306.h"
#include "AvgI2C.h"
#include "AvgFont.h"
#include "math.h"

static uint8_t oledbuf[OLED_BUFFER_SIZE];

void AvgOledInit(uint8_t include_i2c_init, uint8_t inverse,	uint8_t contrast, uint8_t precharge){
	
	uint8_t init_sequence[] = {
		0xAE,			// display OFF
		0xC8,			// COM scan dir (C0 - normal; C8 - remap)
		0x00,			// [low column addr]
		0x10,			// high column addr
		0x00,			// [start line addr]
		0x81, contrast,		// contrast control
		0xA1,			// SEG remap 0-127
		inverse ? 0xA7 : 0xA6,			// A6 - normal; A7 - inverse
		0xA8, 0x3F,			// MUX ratio
		0xA4,			// Entire display ON
		0xD3, 0x00,			// display offset
		0xD5, 0x50,			// display clock divide
		0xD9, precharge,			// precharge period
		0xDA, 0x12,			// COM pin HW-config
		0xDB, 0x20,			// set Vcomh
		0x8D,			// DC-DC enable
		0x14,
		0xAF			// display ON
	};
	
	if(include_i2c_init > 0)
		AvgI2C_Init();
	
	AvgI2C_Write(OLED_I2C_ADDRESS, OLED_I2C_COMMAND, init_sequence, sizeof(init_sequence));
	AvgOledClear();
	AvgOledUpdate();
}

void AvgOledUpdate(){
	uint8_t pagebuf[OLED_WIDTH];
	for(uint8_t page = 0; page < OLED_HEIGHT / 8; page++){
		for(uint32_t i = 0; i < OLED_WIDTH; i++)
			pagebuf[i] = oledbuf[page * OLED_WIDTH + i];
		AvgI2C_Write(OLED_I2C_ADDRESS, OLED_I2C_COMMAND, (uint8_t[]){0x21, 0, OLED_WIDTH - 1}, 3);	// Set start COLUMN (0)
		AvgI2C_Write(OLED_I2C_ADDRESS, OLED_I2C_COMMAND, (uint8_t[]){0x22, page, 7}, 3);						// Set current PAGE
		AvgI2C_Write(OLED_I2C_ADDRESS, OLED_I2C_DATA, pagebuf, sizeof(pagebuf));
	}
}

void AvgOledClear(){
	for(uint32_t i = 0; i < OLED_BUFFER_SIZE; i++)
		oledbuf[i] = 0;
}

void AvgOledClearRect(uint8_t x_first, uint8_t x_last, uint8_t page_first, uint8_t page_last){
	uint8_t x_cnt = 0;
	uint8_t page_cnt = 0;
	for(uint32_t i = 0; i < OLED_BUFFER_SIZE; i++){
		if(x_cnt >= x_first && x_cnt <= x_last && page_cnt >= page_first && page_cnt <= page_last)
			oledbuf[i] = 0;
		if(x_cnt < OLED_WIDTH - 1) x_cnt++;
		else{ x_cnt = 0; page_cnt++; }
	}
}

void AvgOledWriteBuffer(uint8_t *bitmap, uint32_t bitmapSize){
	for(uint32_t i = 0; i < OLED_BUFFER_SIZE; i++)
		oledbuf[i] = bitmap[i];
}

void AvgOledSetXY(uint8_t pixelState, uint8_t x, uint8_t y){
	if(y < OLED_HEIGHT && x < OLED_WIDTH){
		uint8_t addr_page = y / 8;
		uint32_t addr_byte = addr_page * OLED_WIDTH + x;
		uint8_t addr_bit = y%8;
		if(pixelState & 1)
			oledbuf[addr_byte] |= 1 << addr_bit;
		else
			oledbuf[addr_byte] &= ~(1 << addr_bit);
	}
}

void AvgOledPrintChar(uint8_t char_code, uint8_t y_pos_page, uint8_t x_pos_sym){
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, char_code, y_pos_page, x_pos_sym);
}

void AvgOledPrintString(uint8_t *str, uint8_t length, uint8_t y_pos_str, uint8_t x_pos_sym){
	if(length > 0){
		for(uint8_t i = 0; i < length; i++){
			AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, str[i], y_pos_str, x_pos_sym + i);
		}
	}
}

void AvgOledPrintInt(uint8_t y_pos_str, uint8_t x_pos_sym, uint32_t value, uint8_t length){
	uint32_t n = 0, a = 0, ost = 0;
	a = ceil(pow(10, (length)));
	ost = value%a;
	for(uint8_t i = length; i != 0; i--)
	{
		a = ceil(pow(10, (i-1)));
		n = ost/a;
		ost = ost - n*a;
		AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, n + 0x30, y_pos_str, x_pos_sym - i + length);
	}
}

void PrintValueFloat_1dot3(uint8_t y_pos_str, uint8_t x_pos_sym, float value){
	//  V - x_pos_sym
	// [h1],[l1][l2][l3]
	uint8_t h1 = 0;
	uint8_t l1 = 0;
	uint8_t l2 = 0;
	uint8_t l3 = 0;
	uint8_t l3_plus = 0;

	uint32_t uiFull10k = value * 10000;
	l3_plus = (uiFull10k % 10) > 4 ? 1 : 0;
	uiFull10k = uiFull10k/10 + l3_plus;
	l3 = uiFull10k % 10;
	uiFull10k /= 10;
	l2 = uiFull10k % 10;
	uiFull10k /= 10;
	l1 = uiFull10k % 10;
	uiFull10k /= 10;
	h1 = uiFull10k % 10;
	
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, l3 + 0x30, y_pos_str, x_pos_sym + 4);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, l2 + 0x30, y_pos_str, x_pos_sym + 3);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, l1 + 0x30, y_pos_str, x_pos_sym + 2);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, OLED_NUMBER_DIVIDER, y_pos_str, x_pos_sym + 1);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h1 + 0x30, y_pos_str, x_pos_sym);
}

void PrintValueFloat_2dot1(uint8_t y_pos_str, uint8_t x_pos_sym, float value){
	//  V - x_pos_sym
	// [h2][h1],[l1]
	uint8_t h1 = 0;
	uint8_t h2 = 0;
	uint8_t l1 = 0;
	uint8_t l1_plus = 0;

	uint16_t wFull100 = value * 100;
	l1_plus = (wFull100 % 10) > 4 ? 1 : 0;
	wFull100 = wFull100/10 + l1_plus;
	l1 = wFull100 % 10;
	wFull100 /= 10;
	h1 = wFull100 % 10;
	wFull100 /= 10;
	h2 = wFull100 % 10;
	
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, l1 + 0x30, y_pos_str, x_pos_sym + 3);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, OLED_NUMBER_DIVIDER, y_pos_str, x_pos_sym + 2);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h1 + 0x30, y_pos_str, x_pos_sym + 1);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h2 + 0x30, y_pos_str, x_pos_sym);
}

void PrintValueFloat_3dot1(uint8_t y_pos_str, uint8_t x_pos_sym, float value){
	//  V - x_pos_sym
	// [h2][h1],[l1]
	uint8_t h1 = 0;
	uint8_t h2 = 0;
	uint8_t h3 = 0;
	uint8_t l1 = 0;
	uint8_t l1_plus = 0;

	uint16_t wFull100 = value * 100;
	l1_plus = (wFull100 % 10) > 4 ? 1 : 0;
	wFull100 = wFull100/10 + l1_plus;
	l1 = wFull100 % 10;
	wFull100 /= 10;
	h1 = wFull100 % 10;
	wFull100 /= 10;
	h2 = wFull100 % 10;
	wFull100 /= 10;
	h3 = wFull100 % 10;
	
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, l1 + 0x30, y_pos_str, x_pos_sym + 4);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, OLED_NUMBER_DIVIDER, y_pos_str, x_pos_sym + 3);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h1 + 0x30, y_pos_str, x_pos_sym + 2);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h2 + 0x30, y_pos_str, x_pos_sym + 1);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h3 + 0x30, y_pos_str, x_pos_sym);
}

void PrintValueFloat_3(uint8_t y_pos_str, uint8_t x_pos_sym, float value){
	//  V - x_pos_sym
	// [h3][h2][h1]
	uint8_t h1 = 0;
	uint8_t h2 = 0;
	uint8_t h3 = 0;
	uint8_t h1_plus = 0;
	
	uint16_t wFull10 = value * 10;
	h1_plus = (wFull10 % 10) > 4 ? 1 : 0;
	wFull10 = wFull10/10 + h1_plus;
	h1 = wFull10 % 10;
	wFull10 /= 10;
	h2 = wFull10 % 10;
	wFull10 /= 10;
	h3 = wFull10 % 10;
	
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h1 + 0x30, y_pos_str, x_pos_sym + 2);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h2 + 0x30, y_pos_str, x_pos_sym + 1);
	AvgFontPrintChar(oledbuf, OLED_BUFFER_SIZE, OLED_WIDTH, h3 + 0x30, y_pos_str, x_pos_sym);
}
