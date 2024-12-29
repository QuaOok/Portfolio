#include "cmsis_os.h"

#include "init.h"

// Defines

// Global Variables
// HAL Handles


// Thread Handles
osThreadId_t echoThreadHandle;

const osThreadAttr_t echoThread_attributes = {
        .name = "echoThread",
        .priority = (osPriority_t)osPriorityNormal,
        .stack_size = 128 * 4
};

// Queue
osMessageQueueId_t myQueueHandle;

const osMessageQueueAttr_t myQueue_attributes = {
		.name = "myQueue"
};

// Event Handles

// Mutex Handles

// Timer Handles


// Variables
uint8_t rx_char;

// Function declarations
void EchoThread(void *argument);

int main(void){
	Sys_Init();

	// Initialize peripherals
	HAL_UART_Receive_IT(&USB_UART, &rx_char, 1);
	// WARNING! printf support will not work in task 1
	//printf("\033[2J\033[;H");
	uint8_t reset_sequence[] = "\033[2J\033[H";
	HAL_UART_Transmit(&USB_UART, reset_sequence, sizeof(reset_sequence) - 1, HAL_MAX_DELAY);
    fflush(stdout);// Erase screen & move cursor to home position

    osKernelInitialize();

    // Setup RTOS objects
    myQueueHandle = osMessageQueueNew(32, sizeof(uint8_t), &myQueue_attributes);
    echoThreadHandle = osThreadNew(EchoThread, NULL, &echoThread_attributes);

    osKernelStart();


    while (1)
    {
    	// We should never get here
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
	if (huart == &USB_UART) {
		// Put the received character into the queue
		osMessageQueuePut(myQueueHandle, &rx_char, 0, 0);

		// Restart UART reception
		HAL_UART_Receive_IT(&USB_UART, &rx_char, 1);
	}
}

void EchoThread(void *argument) {
    uint8_t char_to_echo;
    while (1) {
        // Wait for a character from the queue
        if (osMessageQueueGet(myQueueHandle, &char_to_echo, NULL, osWaitForever) == osOK) {
            // Echo the character back to the UART
            HAL_UART_Transmit(&USB_UART, &char_to_echo, 1, HAL_MAX_DELAY);
        }
    }
}
