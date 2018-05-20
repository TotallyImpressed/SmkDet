#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32f10x.h" 

void RCC_Config(void);
void GPIO_Config(void);
void SPI_Config(void);
void TIM2_Config(void);
void NVIC_Config(void);
void ADC_Config(void);

#endif
