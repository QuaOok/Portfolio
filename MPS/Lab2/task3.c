//----------------------------------
// Lab 2 - Template file
//----------------------------------

// -- Imports ---------------
//
#include "init.h"

//
//
// -- Prototypes ------------
//
void Init_GPIO();
void Init_Timer7();
void Init_Timer6();

TIM_HandleTypeDef htim6;

int n = 1; // Controls the time interval from 1ms to 100ms
int pero = 100; // Initial period for 1ms (assuming 108MHz clock)
int presc = 1080; // Initial period for 1ms (assuming 108MHz clock)

//
//
// -- Code Body -------------
//

int main() {
	Sys_Init();
	Init_Timer7();
	Init_Timer6();
	Init_GPIO();
	HAL_Init();

	while (1){
		// Main loop can remain empty, as the timer interrupt handles everything
	}
}

//
//
// -- Init Functions ----------
//
void Init_Timer7() {
	NVIC->ISER[55/32] = (uint32_t) 1 << (55 % 32);

	TIM7->PSC = 0x437U;// 1079 : Set prescaler to divide 108MHz clock to 1kHz (1ms tick)
	TIM7->ARR = 0x270FU;// 9999 : Initial period for 100ms

	// Enable update events and generation of interrupt from update

	TIM7->EGR |= 0x1U;
	TIM7->DIER |= 0x001U;

	// Start the timer
	TIM7->CR1 |= 0x1U;
}
void Init_Timer6()
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = presc-1;  // Set prescaler to divide 108MHz clock to 1kHz (1ms tick)
	htim6.Init.Period = pero-1;       // Initial period for 1ms

	__HAL_RCC_TIM6_CLK_ENABLE();    // Enable the TIM6 peripheral clock

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0); // Set the interrupt priority

	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);         // Enable the TIM6 interrupt

	HAL_TIM_Base_Init(&htim6);   // Configure the timer

	HAL_TIM_Base_Start_IT(&htim6); // Start the timer with interrupt enabled
}

void Init_GPIO() {
	GPIO_InitTypeDef InitStructJ;

	InitStructJ.Pin = GPIO_PIN_13;
	InitStructJ.Mode = GPIO_MODE_OUTPUT_PP;  // Set GPIO as output for the LED

	HAL_GPIO_Init(GPIOJ, &InitStructJ);      // Initialize GPIO Pin 13
}

//
//
// -- ISRs (IRQs) -------------
//

void TIM7_IRQHandler() {
	// Clear Interrupt Bit
	if (TIM7->SR & TIM_SR_UIF) {  // Check if update interrupt flag is set
		TIM7->SR &= ~TIM_SR_UIF;  // Clear the update interrupt flag

		// Other code here:
		if (n > 100) {  // Reset when reaching 100ms
			n = 1;
			// Calculate the new period: 999 for 1ms, 99999 for 100ms
		}
		else {
			n++;  // Increment by 1ms each cycle
		}

		__HAL_TIM_SET_AUTORELOAD(&htim6, (pero * n) - 1);
	}
}

void TIM6_DAC_IRQHandler() {
	HAL_TIM_IRQHandler(&htim6);  // Handle the interrupt
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM6) {
		HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_13);  // Toggle LED every n ms
	}
}
