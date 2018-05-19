#include "main.h"
#include "stm32f10x.h"                 

uint16_t data;

int main(void) {
	
	
	data = 200;
	
	RCC_Config();
  GPIO_Config();
	SPI_Config();
	TIM2_Config();
	NVIC_Config();	
	NVIC_EnableIRQ(TIM2_IRQn);
	
	while(1) {
								
		}
}

 void TIM2_IRQHandler(void) {
         if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET) {
					 TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
               GPIO_ResetBits(GPIOA, GPIO_Pin_15);
//							 while ((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)) == RESET)
							 while (!(SPI1->SR & SPI_SR_TXE))
							 {
								 __NOP();
							 }
							 SPI_I2S_SendData(SPI1, data << 4);
//							 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) {}
//							 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP(); 
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();		
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();	
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();		
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();
							 __NOP();							 
							 while (!(SPI1->SR & SPI_SR_RXNE))
							 {
								 __NOP();
							 }
							 GPIO_SetBits(GPIOA, GPIO_Pin_15);   
					 }
}