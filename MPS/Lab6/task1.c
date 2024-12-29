#include "cmsis_os.h"

#include "init.h"

// Defines

// Global Variables
// HAL Handles

// Thread Handles
osThreadId_t RedLEDThreadHandle;
osThreadId_t GreenLEDThreadHandle;
const osThreadAttr_t RedLEDThread_attributes = {
.name = "RedLEDThread",
.priority = (osPriority_t) osPriorityNormal,
.stack_size = 128 * 4
};
const osThreadAttr_t GreenLEDThread_attributes = {
.name = "GreenLEDThread",
.priority = (osPriority_t) osPriorityNormal,
.stack_size = 128 * 4
};




// Queue Handles

// Event Handles

// Mutex Handles

// Timer Handles


// Variables
int RedLED = 0;
int GreenLED = 0;

// Function declarations
void RedLEDThread(void *argument);
void GreenLEDThread(void *argument);

int main(void){
	Sys_Init();

	// Initialize peripherals

	GPIO_InitTypeDef InitStructJ;

	InitStructJ.Pin = GPIO_PIN_5 | GPIO_PIN_13;
	InitStructJ.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOJ, &InitStructJ);

	// WARNING! printf support will not work in task 1
//    printf("\033[2J\033[;H");
//    fflush(stdout);// Erase screen & move cursor to home position

    osKernelInitialize();

    // Setup RTOS objects
    RedLEDThreadHandle = osThreadNew(RedLEDThread, NULL, &RedLEDThread_attributes);
    GreenLEDThreadHandle = osThreadNew(GreenLEDThread, NULL, &GreenLEDThread_attributes);

    osKernelStart();


    while (1)
    {
    	// We should never get here
    }
}

// This is the thread function
void RedLEDThread(void *argument) {
	// Thread initialization code (as needed)
	while(1) {

		if (RedLED == 0) {
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
			RedLED = 1;
		} else {
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
			RedLED = 0;
		}

		// Thread Code...
		osDelay(250); // Delay for 1 tick
	}
}

void GreenLEDThread(void *argument) {
	// Thread initialization code (as needed)
	while(1) {
		if (GreenLED == 0) {
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);
			GreenLED = 1;
		} else {
			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);
			GreenLED = 0;
		}

		// Thread Code...

		osDelay(500); // Delay for 500 ticks

	}
}




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM7)
	{
		// Do not modify this!
		HAL_IncTick();
	}
}


void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&USB_UART);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
}
