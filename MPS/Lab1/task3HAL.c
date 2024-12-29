//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "init.h"

#include<stdint.h>

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)


    // Need to enable clock for peripheral bus on GPIO Port J
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef InitStructJIn;
    GPIO_InitTypeDef InitStructA;
    GPIO_InitTypeDef InitStructD;

    GPIO_InitTypeDef InitStructJOut;
    GPIO_InitTypeDef InitStructF;
    GPIO_InitTypeDef InitStructC;

    InitStructJIn.Pin = GPIO_PIN_5 | GPIO_PIN_13;
    InitStructJIn.Mode = GPIO_MODE_OUTPUT_PP;
    InitStructA.Pin = GPIO_PIN_12;
    InitStructA.Mode = GPIO_MODE_OUTPUT_PP;
    InitStructD.Pin = GPIO_PIN_4;
    InitStructD.Mode = GPIO_MODE_OUTPUT_PP;

    InitStructJOut.Pin = GPIO_PIN_1;
	InitStructJOut.Mode = GPIO_MODE_INPUT;
	InitStructJOut.Pull = GPIO_PULLUP;
	InitStructF.Pin = GPIO_PIN_6;
	InitStructF.Mode = GPIO_MODE_INPUT;
	InitStructF.Pull = GPIO_PULLUP;
	InitStructC.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	InitStructC.Mode = GPIO_MODE_INPUT;
	InitStructC.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(GPIOJ, &InitStructJIn);
    HAL_GPIO_Init(GPIOA, &InitStructA);
    HAL_GPIO_Init(GPIOD, &InitStructD);

    HAL_GPIO_Init(GPIOJ, &InitStructJOut);
	HAL_GPIO_Init(GPIOF, &InitStructF);
	HAL_GPIO_Init(GPIOC, &InitStructC);



    HAL_Delay(1000); // Pause for a second. This function blocks the program and uses the SysTick and
                     // associated handler to keep track of time.

    while(1)
    {
    	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)) {
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
    	} else {
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
    	}

    	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)) {
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);
		}

    	if (HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1)) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		}

    	if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6)) {
    		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
		}
    }
}

