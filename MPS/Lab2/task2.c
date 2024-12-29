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
void Init_Timer();

//
//
// -- Code Body -------------
//

int milliseconds;
int seconds;

int main() {
	Sys_Init();
	Init_Timer();

	SystemClock_Config();

	while (1){
	}
}

//
//
// -- Init Functions ----------
//
void Init_Timer() {
	// Enable the TIM7 interrupt (through NVIC).
	NVIC->ISER[55/32] = (uint32_t) 1 << (55 % 32);

	// Set the timer clock rate and period
	TIM7->PSC = 0x433U;// 1079
	TIM7->ARR = 0x270FU;// 9999

	// Enable update events and generation of interrupt from update
	TIM7->EGR |= 0x1U;
	TIM7->DIER |= 0x001U;

	// Start the timer
	TIM7->CR1 |= 0x1U;
}

//
//
// -- ISRs (IRQs) -------------
//
// Change as needed if not using TIM6
void TIM7_IRQHandler() {
	// Clear Interrupt Bit
	TIM7->SR &= 0x0U;

	// Calculate time elapsed and print it:
	milliseconds++;
	if	(milliseconds == 10) {
		seconds++;
		milliseconds = 0;
	}
	printf("Timer: %ds %d00ms\r", seconds, milliseconds);

}
