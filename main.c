#include "main.h"
#include "stm32f10x.h"                 

uint16_t data_max;
uint16_t data_min;
uint16_t tmprt_data;
uint16_t ilum_data;
void Usart1Put(uint8_t ch);

int main(void) {
	
	
	int i = 0;
	data_min = 0;
	data_max = 50;
	
	RCC_Config();
  GPIO_Config();
	SPI_Config();
	TIM2_Config();
	ADC_Config();
	USART1_Config();
	NVIC_Config();	

	
	

	while(1) {
		Usart1Put(ilum_data);
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
		ilum_data = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2)/10;
		
	}
}

void Usart1Put(uint8_t ch)
{
	    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {
      }
      USART_SendData(USART1, (uint8_t) ch);
      //Loop until the end of transmission
      
}