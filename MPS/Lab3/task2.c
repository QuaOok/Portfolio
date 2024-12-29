#include "init.h"

char input[1];
int closegate = 0;


// Main Execution Loop
int main(void) {

	//Initialize the system
	Sys_Init();

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);

	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(USART6_IRQn);

	HAL_UART_Receive_IT(&USB_UART, (uint8_t*)input, 1);
	HAL_UART_Receive_IT(&MEET_UART, (uint8_t*)input, 1);

	while(1) {
	}
}

void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(&USB_UART);
}

void USART6_IRQHandler(void) {
	HAL_UART_IRQHandler(&MEET_UART);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		HAL_UART_Transmit_IT(&MEET_UART, (uint8_t*)&input[0], 1);
		if ((char)input[0] == '\033' || closegate == 1) {
			if (closegate == 0){
				printf("Exit Character Pressed (ESC)");
			}
		} else {
			printf("%c", (char)input[0]);
			HAL_UART_Receive_IT(&USB_UART, (uint8_t*)input, 1);
		}

	} else if (huart->Instance == USART6) {;
		if ((char)input[0] == '\033') {
			printf("Exit Character Received (ESC)");
			closegate = 1;
		} else {
			printf("%c", (char)input[0]);
			HAL_UART_Receive_IT(&MEET_UART, (uint8_t*)input, 1);
		}
	}
}


