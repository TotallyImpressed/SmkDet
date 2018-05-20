#include "main.h"
#include "stm32f10x.h"                 

uint16_t data_max;
uint16_t data_min;
uint16_t tmprt_data;
uint16_t ilum_data;

int main(void) {
	
	
	
	data_min = 0;
	
	RCC_Config();
  GPIO_Config();
	SPI_Config();
	TIM2_Config();
	ADC_Config();
	NVIC_Config();	
	
	while(1) {
							
		}
}


 void TIM2_IRQHandler(void) {
	 if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET) {                   
		 TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		 ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
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
 
void ADC1_2_IRQHandler(void) {
	
	if (ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET) {
		ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
		tmprt_data = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
		ilum_data = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2);
		data_max = tmprt_data/15;
	}
}