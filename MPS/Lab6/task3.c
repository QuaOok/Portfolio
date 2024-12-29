#include "cmsis_os.h"

#include "init.h"

// Defines

// Global Variables
uint8_t button = 0;
uint8_t j_event = 0;        // Flag for 'j' key press
uint8_t k_event = 0;        // Flag for 'k' key press


// HAL Handles


// Thread Handles
osThreadId_t echoThreadHandle, EventThreadHandle;


const osThreadAttr_t echoThread_attributes = {
        .name = "echoThread",
        .priority = (osPriority_t)osPriorityNormal,
        .stack_size = 128 * 4
};

const osThreadAttr_t EventThread_attributes = {
    .name = "EventThread",
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
void EventThread(void *argument);
void Configure_GPIO();


int main(void){
	Sys_Init();

	// Initialize peripherals
	Configure_GPIO();


	HAL_UART_Receive_IT(&USB_UART, &rx_char, 1);

	uint8_t reset_sequence[] = "\033[2J\033[H";
	HAL_UART_Transmit(&USB_UART, reset_sequence, sizeof(reset_sequence) - 1, HAL_MAX_DELAY);
    fflush(stdout);// Erase screen & move cursor to home position

    osKernelInitialize();

    // Setup RTOS objects
    myQueueHandle = osMessageQueueNew(32, sizeof(uint8_t), &myQueue_attributes);
    echoThreadHandle = osThreadNew(EchoThread, NULL, &echoThread_attributes);
    EventThreadHandle = osThreadNew(EventThread, NULL, &EventThread_attributes);

    osKernelStart();


    while (1)
    {
    	// We should never get here
    }
}

void Configure_GPIO(void) {

    // Configure Pushbutton (PA0) as input with interrupt
    GPIO_InitTypeDef InitStructA_Button;
    InitStructA_Button.Pin = GPIO_PIN_0;
    InitStructA_Button.Mode = GPIO_MODE_IT_RISING;
    InitStructA_Button.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &InitStructA_Button);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0); // Set interrupt priority
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);         // Enable interrupt

    // Configure LED (PA12) as output
    GPIO_InitTypeDef InitStructA_LD3;
    InitStructA_LD3.Pin = GPIO_PIN_12;
    InitStructA_LD3.Mode = GPIO_MODE_OUTPUT_PP;
    InitStructA_LD3.Pull = GPIO_NOPULL;
    InitStructA_LD3.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &InitStructA_LD3);

    // Turn off LED initially
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
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
		if (rx_char == 'j') {
			j_event = 1; // Mark 'j' event
		}
		else if (rx_char == 'k') {
			k_event = 1; // Mark 'k' event
		}
		osMessageQueuePut(myQueueHandle, &rx_char, 0, 0);
		HAL_UART_Receive_IT(&USB_UART, &rx_char, 1);
	}
}

void EXTI0_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_0)
		button = 1;
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

void EventThread(void *argument) {
    while (1) {
        // Wait for both button press and 'j' key event
        if (button && j_event) {
            // Turn on LED (PA12)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
            button = 0;
            j_event = 0;
        }

        // Handle 'k' event (turn off LED and reset states)
        if (k_event) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
            button = 0;
            j_event = 0;
            k_event = 0;
        }

        osDelay(10); // Small delay for polling
    }
}
