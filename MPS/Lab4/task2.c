//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"

void configureADC();
void configureDAC();

ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac;


uint32_t rawValue;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();
	configureDAC();

	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

	while (1) {

		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

		rawValue = HAL_ADC_GetValue(&hadc1);

		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, rawValue);
	}
}


void configureDAC()
{
	// Enable the DAC Clock.
	hdac.Instance = DAC;
	HAL_DAC_Init(&hdac); // Initialize the DAC

	DAC_ChannelConfTypeDef sConfig;

	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
	HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
}


void configureADC()
{
	// Enable the ADC Clock.

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	HAL_ADC_Init(&hadc1); // Initialize the ADC

	ADC_ChannelConfTypeDef config = {0};

	// Configure the ADC channel
	config.Channel = ADC_CHANNEL_6;
	config.Rank = 1;
	config.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	HAL_ADC_ConfigChannel(&hadc1, &config);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hdac)
{

	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin = GPIO_PIN_4;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &InitStruct);

}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hadc)
{

	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin = GPIO_PIN_6;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &InitStruct);

}
