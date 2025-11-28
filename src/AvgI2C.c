/********************************************************************
* @file       : AvgI2C.c
* @author     : Glushanenko Alexander
* @version    : V1.0.0
* @date       : 28.11.2025
* @brief      : I2C basic functions for STM32 -> SSD1306.
* @warning    : PB6 - SCL; PB7 - SDA
********************************************************************/

#include "AvgI2C.h"

void AvgI2C_Init(void)
{
  //PB7 - SDA
	//PB6 - SCL
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
  uint8_t tmp = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
	
  SET_BIT(GPIOB->CRL, GPIO_CRL_CNF7_1 | GPIO_CRL_CNF7_0 | GPIO_CRL_CNF6_1 | GPIO_CRL_CNF6_0 |
                  GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0);
  
  
  CLEAR_BIT(I2C1->OAR2, I2C_OAR2_ENDUAL);
  CLEAR_BIT(I2C1->CR1, I2C_CR1_ENGC);
  CLEAR_BIT(I2C1->CR1, I2C_CR1_NOSTRETCH);
  CLEAR_BIT(I2C1->CR1, I2C_CR1_PE);
	
  //ClockSpeed
  MODIFY_REG(I2C1->CR2, I2C_CR2_FREQ, 36);
	
	#ifdef AVGI2C_SPEED_FAST
		MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE, 11); //400
		MODIFY_REG(I2C1->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), 0x0000801E); //400
	#else
		MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE, 36 + 1); //100
		MODIFY_REG(I2C1->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), 180); //100
	#endif
	
  MODIFY_REG(I2C1->OAR1, I2C_OAR1_ADD0 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD8_9 | I2C_OAR1_ADDMODE, AVGI2C_OWNADDRESS_7BIT);
  MODIFY_REG(I2C1->CR1, I2C_CR1_SMBUS | I2C_CR1_SMBTYPE | I2C_CR1_ENARP, 0x00000000U);
  SET_BIT(I2C1->CR1, I2C_CR1_PE);
  MODIFY_REG(I2C1->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
  MODIFY_REG(I2C1->OAR2, I2C_OAR2_ADD2, 0);
}

void AvgI2C_Write (uint8_t slave_addr, uint8_t first_byte, uint8_t *buf, uint32_t size)
{
  CLEAR_BIT(I2C1->CR1, I2C_CR1_POS);
  MODIFY_REG(I2C1->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
  SET_BIT(I2C1->CR1, I2C_CR1_START);
  while (!READ_BIT(I2C1->SR1, I2C_SR1_SB)){};
  (void) I2C1->SR1;
  //MODIFY_REG(I2C1->DR, I2C_DR_DR, (slave_addr << 1) | I2C_REQUEST_WRITE);
		MODIFY_REG(I2C1->DR, I2C_DR_DR, slave_addr << 1);
  while (!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)){};
  (void) I2C1->SR1;
  (void) I2C1->SR2;
		
	MODIFY_REG(I2C1->DR, I2C_DR_DR, first_byte);
	while (!READ_BIT(I2C1->SR1, I2C_SR1_TXE)){};
		
	for(uint32_t i = 0; i < size; i++)
	{
		MODIFY_REG(I2C1->DR, I2C_DR_DR, buf[i]);
		while (!READ_BIT(I2C1->SR1, I2C_SR1_TXE)){};
	}
	SET_BIT(I2C1->CR1, I2C_CR1_STOP);
}
