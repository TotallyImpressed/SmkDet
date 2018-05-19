#include "main.h"
#include "stm32f10x.h"                 

uint16_t data_max;
uint16_t data_min;

int main(void) {
	
	
	data_max = 200;
	data_min = 0;
	
	RCC_Config();
  GPIO_Config();
	SPI_Config();
	TIM2_Config();
	NVIC_Config();	
	
	while(1) {
								
		}
}

 void TIM2_IRQHandler(void) {
	 if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET) {                   
		 TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
			 GPIO_ResetBits(GPIOA, GPIO_Pin_15);                             			//SYNC on external DAC to negedge (enable writing to DAC)
			 if ((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)) == SET)
				 SPI_I2S_SendData(SPI1, data_max << 4);                        			//sending HIGH voltage level to DAC
			 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
			 GPIO_SetBits(GPIOA, GPIO_Pin_15);                               			//SYNC on external DAC to posedge (disable writing to DAC)
			 while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)) == RESET) {}
			 GPIO_ResetBits(GPIOA, GPIO_Pin_15);																	//SYNC on external DAC to negedge
			 if ((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)) == SET)						
				 SPI_I2S_SendData(SPI1, data_min << 4);															//sending LOW voltage level to DAC
			 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
			 GPIO_SetBits(GPIOA, GPIO_Pin_15);																		//SYNC on external DAC to posedge (disable writing to DAC)
	}
}