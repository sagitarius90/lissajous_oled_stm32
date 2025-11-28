/********************************************************************
* @file       : main.c
* @author     : Glushanenko Alexander
* @version    : V1.0.0
* @date       : 28.11.2025
* @brief      : Lissajous figures generate and rendering
*
* Hardware:
* MCU: STM32F103C8 (with 8MHz crystal oscillator)
* Display: OLED SSD1306 128x64
*
* GPIO configuration:
* PB0 - Start button (with 10K pull-up)
* PB6 - I2C SDA
* PB7 - I2C SCL
********************************************************************/


#include "stm32f10x.h"
#include <stdint.h>
#include <math.h>
#include "AvgOledSSD1306.h"

#define BTN_START_BIT			0 // GPIOB
#define M_PI							3.141593f
#define M_2_PI						6.283185f

static void GPIO_Init(){
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	// B0 - as input floating
	GPIOB->CRL |= GPIO_CRL_CNF0_0;
	GPIOB->CRL &= ~GPIO_CRL_CNF0_1;
	GPIOB->CRL &= ~GPIO_CRL_MODE0;
}

static uint8_t StartButtonPressed(){
	return !((GPIOB->IDR >> BTN_START_BIT) & 1);
}

static void delay_ms(uint32_t _delay)
{
	for(uint32_t i = 0; i < _delay; i++){
		for(uint32_t j = 0; j < 8000; j++) __nop();
	}
}
												
static void printLissajous(const uint8_t x_center, const uint8_t y_center, const float fAmp, 
	const uint32_t xfreq, const float fXphase, const uint32_t yfreq, const float fYphase){
	
	uint8_t x = x_center;
	uint8_t y = y_center;
	float fTime = 0;
	float fX = 0;
	float fY = 0;
	while(!StartButtonPressed()){
		fX = fAmp * sinf(xfreq * fTime + fXphase);
		fY = fAmp * sinf(yfreq * fTime + fYphase);
		x = (int8_t)roundf(fX) + x_center;
		y = (int8_t)roundf(fY) + y_center;
		fTime += 0.025f;
		AvgOledSetXY(1, x, y);
		AvgOledUpdate();
	}
	AvgOledClear();
	AvgOledUpdate();
	while(StartButtonPressed()) continue; // wait for button release
}
	
static void printLissajousRotary(const uint8_t x_center, const uint8_t y_center, const float fAmp, 
	const uint32_t xfreq, const uint32_t yfreq){
	
	uint8_t x = x_center;
	uint8_t y = y_center;
	float fPhase = 0;
	float fX = 0;
	float fY = 0;
	float fOmegaX = M_2_PI * xfreq;
	float fOmegaY = M_2_PI * yfreq;
	float fFreqMin = xfreq < yfreq ? xfreq : yfreq;
	uint8_t x_first_to_clear = x_center - (uint8_t)ceil(fAmp);
	uint8_t x_last_to_clear = x_center + (uint8_t)ceil(fAmp);
	uint8_t page_first_to_clear = 0;
	uint8_t page_last_to_clear = 7;
	while(!StartButtonPressed()){
		for(float fTime1 = 0; fTime1 < (2 / fFreqMin); fTime1 += 0.005f){
			fX = fAmp * sinf(fOmegaX * fTime1  + fPhase);
			fY = fAmp * sinf(fOmegaY * fTime1);
			x = (int8_t)roundf(fX) + x_center;
			y = (int8_t)roundf(fY) + y_center;
			AvgOledSetXY(1, x, y);
		}
		fPhase += 0.05f;
		AvgOledUpdate();
		AvgOledClearRect(x_first_to_clear, x_last_to_clear, page_first_to_clear, page_last_to_clear);
	}
	AvgOledClear();
	AvgOledUpdate();
	while(StartButtonPressed()) continue; // wait for button release
}

int main(){
	GPIO_Init();
	delay_ms(100); // for OLED correct init
	AvgOledInit(1, 0, 0xff, 0x50);
	AvgOledClear();
	
	uint8_t str1[] = "PRESS START";
	uint8_t str2[] = "TO BEGIN...";
	AvgOledPrintString(str1, sizeof(str1), 2, 0);
	AvgOledPrintString(str2, sizeof(str2), 4, 0);
	AvgOledUpdate();
	
	while(!StartButtonPressed()) continue; // wait for push button
	while(StartButtonPressed()) continue; // wait for button release
	
	AvgOledClear();
	AvgOledUpdate();

	uint8_t x_center = 64;
	uint8_t y_center = 32;
	float amplitude = 30;
	
	AvgOledPrintString((uint8_t*)"L1", 2, 0, 0);
	printLissajous(x_center, y_center, amplitude, 1, 0.0, 1, 0.0);
	
	AvgOledPrintString((uint8_t*)"L1R", 3, 0, 0);
	printLissajousRotary(x_center, y_center, amplitude, 1, 1);
	
	AvgOledPrintString((uint8_t*)"L2", 2, 0, 0);
	printLissajous(x_center, y_center, amplitude, 2, 0.0, 3, 0.0);
	
	AvgOledPrintString((uint8_t*)"L2R", 3, 0, 0);
	printLissajousRotary(x_center, y_center, amplitude, 2.0, 3.0);
	
	AvgOledPrintString((uint8_t*)"L3", 2, 0, 0);
	printLissajous(x_center, y_center, amplitude, 1, 0.0, 2, 0.0);
	
	AvgOledPrintString((uint8_t*)"L3R", 3, 0, 0);
	printLissajousRotary(x_center, y_center, amplitude, 1, 2);
	
	AvgOledPrintString((uint8_t*)"L4", 2, 0, 0);
	printLissajous(x_center, y_center, amplitude, 2, 0.0, 5, 0.0);
	
	AvgOledPrintString((uint8_t*)"L4R", 3, 0, 0);
	printLissajousRotary(x_center, y_center, amplitude, 2, 5);
	
	// ---------------------------------------------------------
	
	AvgOledClear();
	AvgOledUpdate();

	return 0;
}
