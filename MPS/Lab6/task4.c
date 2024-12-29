#include "cmsis_os.h"

#include "init.h"

// Defines

// Global Variables
// HAL Handles

// Thread Handles
osThreadId_t TimerThreadHandle;
const osThreadAttr_t TimerThread_attributes = {
.name = "TimerThread",
.priority = (osPriority_t) osPriorityNormal,
.stack_size = 128 * 4
};

// Queue Handles

// Event Handles

// Mutex Handles

// Timer Handles
osTimerId_t periodic_id;

// Variables
uint32_t flags;
uint8_t message[] = "Hey! 2 seconds have passed\r\n";

// Function declarations
void TimerThread(void *argument);
static void periodic_Callback(void *argument);

int main(void){
	Sys_Init();

	// Initialize peripherals
	periodic_id = osTimerNew(periodic_Callback, osTimerPeriodic, (void *)5, NULL);

    osKernelInitialize();

    // Setup RTOS objects
    TimerThreadHandle = osThreadNew(TimerThread, NULL, &TimerThread_attributes);

    osKernelStart();


    while (1)
    {
    	// We should never get here
    }
}

// This is the thread function
void TimerThread(void *argument) {
	// Thread initialization code (as needed)
	osTimerStart(periodic_id, 2000);
	while(1) {
		flags = osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever);
		HAL_UART_Transmit(&USB_UART, message, sizeof(message), HAL_MAX_DELAY);
	}
}

static void periodic_Callback (void *argument) {
  // do something, i.e. set thread/event flags
  flags = osThreadFlagsSet(TimerThreadHandle, 0x0001U);
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
