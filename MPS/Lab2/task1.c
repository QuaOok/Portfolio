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

int ext1 = 0;
int ext7 = 0;

int main() {
	Sys_Init();
	Init_GPIO();

	SystemClock_Config();

	while (1){
		if(ext1) {
			printf("EXT1 triggered\r\n");
			ext1 = 0;
		}
		if(ext7) {
			printf("EXT7 triggered\r\n");
			ext7 = 0;
		}
	}
}

//
//
// -- Init Functions ----------
//
void Init_GPIO() {
	//HAL
	GPIO_InitTypeDef InitStructJ; //Interrupt

	InitStructJ.Pin = GPIO_PIN_1;
	InitStructJ.Mode = GPIO_MODE_IT_RISING;
	InitStructJ.Pull = GPIO_PULLDOWN;

	HAL_GPIO_Init(GPIOJ, &InitStructJ);

	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	//registers
	SYSCFG->EXTICR[1] |= 0x5000U;

	GPIOF->PUPDR |= 0x8000U; //PULL DOWN for PF7(D6)

	NVIC->ISER[23/32] = (uint32_t) 1 << (23 % 32);;

	EXTI->IMR |= GPIO_PIN_7;
	EXTI->RTSR |= GPIO_PIN_7;
}

//
//
// -- ISRs (IRQs) -------------
//

// Non-HAL GPIO/EXTI Handler
void EXTI9_5_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI->PR = GPIO_PIN_7;

	// Other code here:
	ext7 = 1;
	for (int i = 0; i < 10; i++) {
		asm("nop");
	}
}

//HAL - GPIO/EXTI Handler
void EXTI1_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_1)
		ext1 = 1;
	for (int i = 0; i < 10; i++) {
			asm("nop");
	}
}
