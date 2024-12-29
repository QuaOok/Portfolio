//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>

char choice;
//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout);

    // Need to enable clock for peripheral bus on GPIO Port J
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;//through registers
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;


    GPIOJ->MODER |= 0x400U; //Bitmask for GPIO J Pin 5 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
    GPIOJ->MODER |= 0x4000000U;
    GPIOA->MODER |= 0x1000000U;
    GPIOD->MODER |= 0x100U;


    GPIOF->PUPDR |= 0x1000U;
    GPIOJ->PUPDR |= 0x4U;
    GPIOC->PUPDR |= 0x1000U;
    GPIOC->PUPDR |= 0x4000U;


    HAL_Delay(1000); // Pause for a second. This function blocks the program and uses the SysTick and
                     // associated handler to keep track of time.
    while(1)
    {
    	if (GPIOC->IDR & (uint16_t)GPIO_PIN_7) { //C7 to J13
    		GPIOJ->BSRR = (uint32_t)GPIO_PIN_13;
		} else {
			GPIOJ->BSRR = (uint32_t)GPIO_PIN_13 << 16;
		}

    	if (GPIOC->IDR & (uint16_t)GPIO_PIN_6) {
			GPIOJ->BSRR = (uint32_t)GPIO_PIN_5;
		} else {
			GPIOJ->BSRR = (uint32_t)GPIO_PIN_5 << 16;
		}
    	if (GPIOJ->IDR & (uint16_t)GPIO_PIN_1) {
			GPIOA->BSRR = (uint32_t)GPIO_PIN_12;
		} else {
			GPIOA->BSRR = (uint32_t)GPIO_PIN_12 << 16;
		}
    	if (GPIOF->IDR & (uint16_t)GPIO_PIN_6) {
			GPIOD->BSRR = (uint32_t)GPIO_PIN_4 << 16;
		} else {
			GPIOD->BSRR = (uint32_t)GPIO_PIN_4;
		}
    }
}

