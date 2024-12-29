//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

#include "init.h"

char input_1[1];
char input_6[1];

// Main Execution Loop
int main(void) {

	//Initialize the system
	Sys_Init();

	while(1) {
		if (HAL_UART_Receive(&USB_UART, (uint8_t *)input_1, 1, 0x01) != HAL_TIMEOUT) {
			uart_putchar(&MEET_UART, &input_1[0]);
			if ((char)input_1[0] == '\033') {
				printf("Exit Character Pressed (ESC)");
				return 1;
			} else {
				printf("%c", (char)input_1[0]);
			}
		}
		if (HAL_UART_Receive(&MEET_UART, (uint8_t *)input_6, 1, 0x01) != HAL_TIMEOUT) {
			if ((char)input_6[0] == '\033') {
				printf("Exit Character Pressed (ESC)");
				return 1;
			} else {
				printf("%c", (char)input_6[0]);
			}
		}
	}
}


