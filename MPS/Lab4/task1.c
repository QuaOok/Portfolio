//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"

void configureADC();
void Init_Timer6();

TIM_HandleTypeDef htim6;
ADC_HandleTypeDef hadc1;
uint32_t rawValue;
float rawValues[100];
float floatValue;
int track = 0;
int convCompleted;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	Init_Timer6();
	configureADC();

	printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);

	// Code goes here

	HAL_TIM_Base_Start_IT(&htim6); // Start the timer with interrupt enabled

	while (1) {
		if (convCompleted) {
			if (track == 100) {
				track = 0;
			}

			HAL_ADC_Start(&hadc1);

			HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); //timeout of 100ms

			rawValue = HAL_ADC_GetValue(&hadc1);
			floatValue = ((float)rawValue) / 4095 * 3300;
			rawValues[track] = floatValue;

			float min = rawValues[0];
			float max = rawValues[0];
			float average = 0;

			for (int i = 1; i < 100; i++) {
				if (rawValues[i] < min) {
					min = rawValues[i];
				}
				if (rawValues[i] > max) {
					max = rawValues[i];
				}
				average += rawValues[i];
			}

			printf("%lx %.0f mV       \r\n", rawValue, floatValue);
			printf("Average:%.0f mV    \r\n", average/100);
			printf("Min:%.0f mV Max:%.0f mV     \033[H", min, max);

			track += 1;
			convCompleted = 0;
		}
	}
}

void Init_Timer6()
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 10800-1;  //  108MHz / 10800 Hz = 10,000 Hz
	htim6.Init.Period = 1000-1;       // 10000 Hz / 1000 Hz = 10 Hz = 100ms

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0); // Set the interrupt priority

	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);         // Enable the TIM6 interrupt

	HAL_TIM_Base_Init(&htim6);   // Configure the timer


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
	/* Available sampling times:

		ADC_SAMPLETIME_3CYCLES
	  	ADC_SAMPLETIME_15CYCLES
		ADC_SAMPLETIME_28CYCLES
		ADC_SAMPLETIME_56CYCLES
		ADC_SAMPLETIME_84CYCLES
		ADC_SAMPLETIME_112CYCLES
		ADC_SAMPLETIME_144CYCLES
		ADC_SAMPLETIME_480CYCLES

	*/

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

void TIM6_DAC_IRQHandler() {
	HAL_TIM_IRQHandler(&htim6);  // Handle the interrupt
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//	printf("hey");
	convCompleted = 1;
}
