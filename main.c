#include "main.h"
#include "stm32f10x.h"  
#include <stdio.h>
#define USART_BUF 8

uint16_t data_max,data_min;
uint16_t tmprt_data, avg_tmp;
uint8_t switchState = 0, DMA_cmplt_flag, cal_flag;
uint16_t j,k = 1,i = 1;
uint32_t max,amp,sum_cal;
double rel_amp = 0, sum = 0;
double arr_amp[100];
uint16_t zero = 3020,avg_amp = 0;
uint16_t ADC_values[256],max_arr[100],tmprt_arr[50];
uint16_t cal_cnt = 0,amp_cnt = 0,tmprt_cnt = 0, tmprt_sum = 0;
char info[USART_BUF];
int main(void) {

	data_min = 0;        //Low DAC value(SPI)
	data_max = 200;			 //High DAC value(SPI)
	
	//---Initialization--//
	RCC_Config();
  GPIO_Config();
	SPI_Config();
	TIM2_Config();
	TIM3_Config();
	ADC_Config();
	NVIC_Config();	
	ADC_DMA_Init();
	USART1_Config();
	
	
	
	while(1) {
		if(cal_flag == 0) {                           //start of calibration to maximum signal value
			while(cal_cnt<1000) {
				max = (max_search(&ADC_values[0])-zero);
				max_arr[cal_cnt] = max;
				if(cal_cnt == 998)
					max = (max_search(&max_arr[0])-zero);
				cal_cnt++;
			}
			cal_flag = 1;
		}
		if((DMA_cmplt_flag == 1) && (cal_flag == 1)) {	//calculating relative amplitude 
			rel_amp = 1000*(max_search(&ADC_values[0])-zero)/max; //based on calibration maximum value (light)
			arr_amp[j] = rel_amp;
			j++;
			if(j == 99) {
				j = 0;
				for(amp_cnt=0;amp_cnt<100;amp_cnt++)       //averaging relative amplitude 
					sum = arr_amp[amp_cnt] + sum;
				avg_amp = (uint16_t)(sum/99);
				sum = 0;
			}
			if(i == 128)																//counter to receive data from
				i = 1;																		//2d regular ADC channel(temperature)
			else
				i++;
			DMA_cmplt_flag = 0;
		}
		sprintf(info, "%d:%d", avg_amp/10, avg_tmp);
		USART_SendWord(info);
	}			
}


void DMA1_Channel1_IRQHandler(void) {
	if(DMA_GetITStatus(DMA1_IT_TC1) == SET) {
		DMA_ClearITPendingBit(DMA1_IT_TC1);	
	}
	DMA_cmplt_flag = 1;
	tmprt_data = (uint16_t)(ADC_values[2*i-1]*0.806 - 1375)/22.5;   //calculating temperature value with coefficients from AD22100KTZ
	tmprt_arr[k] = tmprt_data;																			
	if(k == 50) {																										//averaing temperature values
		k = 0;
		tmprt_arr[0] = 0;																							//shameful crutch
		tmprt_cnt = 0;
		tmprt_sum = 0;
		for(tmprt_cnt=0;tmprt_cnt<49;tmprt_cnt++)
			tmprt_sum = tmprt_sum + tmprt_arr[tmprt_cnt];
	}
	k++;
	avg_tmp = tmprt_sum/49;
	
	}


 void TIM2_IRQHandler(void) {
	 if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET) {
		 TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		 if ((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)) == SET) {		//managing of LEDmodulation amplitude
				 GPIO_ResetBits(GPIOA, GPIO_Pin_15);												//through SPI with TIMER2 set frequency
				 if(switchState)
					SPI_I2S_SendData(SPI1, data_max << 4); 
				 else if(!switchState)
					SPI_I2S_SendData(SPI1, data_min << 4); 
			 }
			 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
			 GPIO_SetBits(GPIOA, GPIO_Pin_15);
			 switchState = ~switchState;
	}
}
 
void ADC1_2_IRQHandler(void) {
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET) {
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}

void USART1_IRQHandler(void) {
		if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
			USART_SendData(USART1, 'A');
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
}

uint16_t max_search(uint16_t *s_arr) {
	uint16_t max = *s_arr;
	for(uint16_t i = 1;i < 100; i++) {
		if(*(s_arr+i) > max)
			max = *(s_arr+i);
		else
			max = max;
	}
	return max;
}

void ADC_DMA_Init() {
	DMA_InitTypeDef DMA_Init_Structure;
	DMA_StructInit(&DMA_Init_Structure);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_Init_Structure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
	DMA_Init_Structure.DMA_BufferSize = 256;
	DMA_Init_Structure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init_Structure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_Init_Structure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init_Structure.DMA_MemoryBaseAddr = (uint32_t)ADC_values;
	DMA_Init_Structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init_Structure.DMA_Mode = DMA_Mode_Circular;
	DMA_Init_Structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_Init_Structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init_Structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	DMA_Init(DMA1_Channel1, &DMA_Init_Structure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	ADC_DMACmd(ADC1,ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void USART_SendChar(char c){
	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET)
		USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {
	}
}

void USART_SendWord(char *word) {
	for(int i=0;i<USART_BUF;i++)
		USART_SendChar(word[i]);
	USART_SendData(USART1, '\n');
	USART_SendData(USART1, '\r');
}

