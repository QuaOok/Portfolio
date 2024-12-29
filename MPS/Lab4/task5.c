#include "init.h"

void configureADC();
void configureDAC();
void Init_Timer();

ADC_HandleTypeDef hadc1, hadc2;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim6;


uint32_t adcValue1;
uint32_t adcValue2;

float sampleValues[5] = {0};
float prevValues[4] = {0};
float outValue;

uint8_t adc1Completed = 0;
uint8_t adc2Completed = 0;

uint8_t dc_bias_adc = 1;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();
	configureDAC();
	Init_Timer();

	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);

	// Code goes here
	HAL_TIM_Base_Start(&htim6); // Start the timer with interrupt enabled


	HAL_ADC_Start_IT(&hadc1);
	HAL_ADC_Start_IT(&hadc2);

	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

	while (1) {
		if(adc1Completed && adc2Completed)
		{
			adc1Completed = 0;
			adc2Completed = 0;

			adcValue1 = HAL_ADC_GetValue(&hadc1);
			adcValue2 = HAL_ADC_GetValue(&hadc2);

			float voltage1 = ((float)adcValue1 / 4095.0) * 3300; //- dc_bias_adc;
			float voltage2 = ((float)adcValue2 / 4095.0) * 3300; //- dc_bias_adc;

			float mixedSignal = voltage1 * voltage2;// * 4095/3300;

			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, mixedSignal);
/*
			sampleValues[4] = sampleValues[3];
			sampleValues[3] = sampleValues[2];
			sampleValues[2] = sampleValues[1];
			sampleValues[1] = sampleValues[0];
			sampleValues[0] = mixedSignal;

			outValue = (0.001 * sampleValues[0]) + (0.002 * sampleValues[2]) + (0.001 * sampleValues[4])
			                     + (3.166 * prevValues[0]) - (4.418 * prevValues[1]) - (3.028 * prevValues[2]) - (0.915 * prevValues[3]);

			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, outValue + dc_bias_adc);

			for(uint8_t i = 3; i > 0; i--)
			{
				prevValues[i] = prevValues[i-1];
			}
			prevValues[0] = outValue;
			*/
		}
	}
}

void configureADC()
{
	// Enable the ADC1 Clock.
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T6_TRGO;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	HAL_ADC_Init(&hadc1); // Initialize the ADC

	ADC_ChannelConfTypeDef config = {0};

	// Configure the ADC channel 1
	config.Channel = ADC_CHANNEL_6;
	config.Rank = 1;
	config.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	HAL_ADC_ConfigChannel(&hadc1, &config);

	// Enable the ADC1 Clock.
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
	hadc2.Init.ContinuousConvMode = ENABLE;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T6_TRGO;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	HAL_ADC_Init(&hadc2); // Initialize the ADC

	ADC_ChannelConfTypeDef config2 = {0};

	// Configure the ADC channel 2
	config2.Channel = ADC_CHANNEL_12;
	config2.Rank = 1;
	config2.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	HAL_ADC_ConfigChannel(&hadc2, &config2);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	// GPIO init
	GPIO_InitTypeDef InitStruct1;
	InitStruct1.Mode = GPIO_MODE_ANALOG;
	InitStruct1.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &InitStruct1);

	GPIO_InitTypeDef InitStruct2;
	InitStruct2.Mode = GPIO_MODE_ANALOG;
	InitStruct2.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOC, &InitStruct2);

}

void configureDAC()
{
	// Enable the DAC Clock.
	hdac.Instance = DAC;
	HAL_DAC_Init(&hdac); // Initialize the DAC

	DAC_ChannelConfTypeDef sConfig;

	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hadc)
{
	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin = GPIO_PIN_4;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &InitStruct);

}

void ADC_IRQHandler(void) {
	if(adc1Completed == 0){
		HAL_ADC_IRQHandler(&hadc1);
	}
	else if(adc2Completed == 0)
	{
		HAL_ADC_IRQHandler(&hadc2);
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        adc1Completed = 1;
    } else if (hadc->Instance == ADC2) {
        adc2Completed = 1;
    }
}

void Init_Timer()
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	//(108 * 10) / 108M Hz == 100 kHz
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 108-1;
	htim6.Init.Period = 10-1;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim6);   // Configure the timer

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
}
