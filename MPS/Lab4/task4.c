#include "init.h"

void configureADC();
void configureDAC();

ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac;

uint32_t rawValue;
float sampleValues[3];
float floatValue;
float outValue;
float previousOut = 0;

float w1 = 0.312500;
float w2 = 0.240385;
float w3 = 0.296875;


// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();
	configureDAC();

	printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);

	// Code goes here
	HAL_ADC_Start(&hadc1);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

	while (1) {
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

		rawValue = HAL_ADC_GetValue(&hadc1);
		floatValue = ((float)rawValue) / 4095 * 3300;
		sampleValues[0] = sampleValues[1];
		sampleValues[1] = sampleValues[2];
		sampleValues[2] = floatValue;
		outValue = 0;
//		outValue = w1 * sampleValues[2] + w2 * sampleValues[1] + w1 * sampleValues[0] + w3 * previousOut;

		asm("VMLA.F32 %[dest],%[fac1],%[fac2]" : [dest] "+w" (outValue) : [fac1] "w" (w1) , [fac2] "w" (sampleValues[2]));
		asm("VMLA.F32 %[dest],%[fac1],%[fac2]" : [dest] "+w" (outValue) : [fac1] "w" (w2) , [fac2] "w" (sampleValues[1]));
		asm("VMLA.F32 %[dest],%[fac1],%[fac2]" : [dest] "+w" (outValue) : [fac1] "w" (w1) , [fac2] "w" (sampleValues[0]));
		asm("VMLA.F32 %[dest],%[fac1],%[fac2]" : [dest] "+w" (outValue) : [fac1] "w" (w3) , [fac2] "w" (previousOut));


		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, outValue);

		previousOut = outValue;
	}
}

void configureADC()
{
	// Enable the ADC Clock.
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ContinuousConvMode = ENABLE;
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


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &InitStruct);

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

void HAL_DAC_MspInit(DAC_HandleTypeDef *hadc)
{
	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin = GPIO_PIN_4;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &InitStruct);

}
