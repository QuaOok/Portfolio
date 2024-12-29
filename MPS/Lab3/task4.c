

//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"

// If needed:
//#include <stdio.h>
//#include <stdlib.h>

/*
 * For convenience, configure the SPI handler here
 */

SPI_HandleTypeDef hspi2;

#define BUFFER_SIZE 128
char topHalfBuffer[BUFFER_SIZE] = {0};
char bottomHalfBuffer[BUFFER_SIZE] = {0};
int topHalfPos = 0;
int bottomHalfPos = 0;

uint16_t txData;
uint16_t rxData;

uint8_t input;

uint8_t closegate = 0;

uint8_t reverse_bits(uint8_t n);

uint8_t rec = 0;
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c

void Init_GPIO() {
	//HAL
	GPIO_InitTypeDef InitStructJ; //Interrupt

	InitStructJ.Pin = GPIO_PIN_1;
	InitStructJ.Mode = GPIO_MODE_IT_RISING;
	InitStructJ.Pull = GPIO_PULLDOWN;

	HAL_GPIO_Init(GPIOJ, &InitStructJ);

	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

//HAL - GPIO/EXTI Handler
void EXTI1_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_1)
		//ext1 = 1;
	for (int i = 0; i < 10; i++) {
			asm("nop");
	}
}

void configureSPI()
{
//	[SPIHandler here].Instance = SPI2; // Please use SPI2!
//	[SPIHandler here].Init.Mode = SPI_MODE_MASTER; // Set master mode
//	[SPIHandler here].Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
/*
 * ... You get the idea.
 */
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; //To operate 100k Hz (13.5 / 128 == 0.1)
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;

	if( HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		printf("SPI configuration error");
		while(1);
	}
	//
	// Note: HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
	//
	// HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properties of
	// the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
	// the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
 // Do NOT change the name of an MspInit function; it needs to override a
 // __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// SPI GPIO initialization structure here
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitTypeDef  GPIO_InitStruct_CS;

	if (hspi->Instance == SPI2)
	{
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.


		// Initialize CS Pin (PJ4)
		GPIO_InitStruct_CS.Pin       = GPIO_PIN_4;
		GPIO_InitStruct_CS.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_CS.Pull      = GPIO_NOPULL;
		GPIO_InitStruct_CS.Speed     = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct_CS);

		// Initialize SCK Pin (PA12)
		GPIO_InitStruct.Pin       = GPIO_PIN_12;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Initialize  COPI Pin (PB15)
		GPIO_InitStruct.Pin       = GPIO_PIN_15;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;

		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// Initialize CIPO Pin (PB14)
		GPIO_InitStruct.Pin 	  = GPIO_PIN_14;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0); // Set the interrupt priority

		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
}

void displayBuffer(void)
{
	closegate = 0;
    printf("\033[2J\033[;H");
    fflush(stdout);

    printf("Top half (Sent):\n%s\n", topHalfBuffer);
    printf("\033[11;1H"); // Move to the bottom half of the terminal
    printf("Bottom half (Received from SPI):\n%s\n", bottomHalfBuffer);
}

void firmware(void)
{
    uint16_t txData = 0x4000U;  // Read request for firmware version register (register 8)
    uint16_t rxData;

    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);

    uint8_t bits = rxData & 0xff;
    uint8_t version = reverse_bits(bits);

    printf("Firmware Version: %d.%d\r\n", (version >> 4) & 0x0F, version & 0x0F);
    fflush(stdout);
}

void get_temperature(void)
{
    //uint16_t txData, rxData;
    uint16_t temperature_raw;
    int16_t temperature_celsius;

    // Step 1: Check whether temperature is ready to read
    txData = 0x00U;
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);

    if((rxData & 0x8U) == 0)
    {
    	txData = (0x88U << 8) | 0x40U;
    	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    	HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
    	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);

        // Step 2: Poll the STS_REG (register 0) for the TRDY bit
        do {
            txData = 0x00U;
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY);
            HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
            printf("waiting...\r\n");
        } while ((rxData & 0x8U) == 0);  // Check if TRDY bit (bit 4) is set
    }

    // Step 3: Read TMP_LO and TMP_HI to retrieve the temperature value
    txData = 0x2000U;  // Read TMP_LO (Register 4)
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
    uint8_t tmp_low = rxData; // Store low byte

    txData = 0x2800U;  // Read TMP_HI (Register 5)
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&txData, (uint8_t*)&rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
    uint8_t tmp_high = rxData << 8; // Combine high byte with low byte

    temperature_raw = (tmp_high << 8) | tmp_low;
    temperature_raw &= 0x0FFF;
    temperature_celsius = 357.6 - (0.187 * temperature_raw);

    printf("temperature: %x\r\n", temperature_raw);


    printf("Temperature: %d.%d C\r\n", temperature_celsius / 10, abs(temperature_celsius % 10));
    fflush(stdout);
}

void reset_peripheral_terminal(void)
{
    txData = (0x88U << 8) | 0x20U;

    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);

    printf("Peripheral Terminal Reset\r\n");
    fflush(stdout);
}

void rw_id(void)
{
    // Step 1: Unlock the DEVID register by setting ULKDID (bit 7) in CTL_REG (register 1)
	txData = (0x88U << 8) | 0x1U;
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);


    uint8_t new_id = 0x64U; //987
    //new_id = reverse_bits(new_id);

    txData = (0xC8U << 8) | new_id;

    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);

    // Step 2: Read the DEVID register (register 9)
    txData = 0x4800U;  // Read DEVID register
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

    new_id = rxData & 0xFFU;
    //new_id = reverse_bits(new_id);
    printf("Device ID: %d\r\n", new_id);
    fflush(stdout);
}

void menu(void)
{
	closegate = 1;
    printf("\033[2J\033[;H");
    printf("Menu:\r\n");
    printf("1. Read Firmware Version\r\n");
    printf("2. Trigger Temperature Measurement\r\n");
    printf("3. Reset Peripheral Terminal\r\n");
    printf("4. Read/Change Device ID\r\n");
    printf("5. Return Home\r\n");
    fflush(stdout);
}


uint8_t reverse_bits(uint8_t n) {
    uint8_t reversed = 0;
    for (int i = 0; i < 8; i++) {
        // Shift left reversed and add the LSB of n
        reversed = (reversed << 1) | (n & 1);
        // Shift right n to process the next bit
        n >>= 1;
    }
    return reversed;
}

void send_character(uint8_t alph)
{
	rec = 1;
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
	uint8_t reversed = reverse_bits(alph);

	txData = (0xB0U << 8) | reversed;

	if (HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY) == HAL_OK)
	{
		if (topHalfPos < BUFFER_SIZE - 1)
		{
			topHalfBuffer[topHalfPos++] = alph;
			topHalfBuffer[topHalfPos] = '\0';
		}
	}
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
	displayBuffer();

}

void receive_character()
{
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);

	txData = (0x30U << 8);

	volatile HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY);
	if (res == HAL_OK)
	{
		//uint8_t reversed = reverse_bits(rxData << 8);
		rxData &= 0xFF;
		uint8_t received = rxData;

		if ((received) != 0x00U)
		{
			if (bottomHalfPos < BUFFER_SIZE - 1)
			{
				received = reverse_bits(received);
				bottomHalfBuffer[bottomHalfPos++] = received;
				bottomHalfBuffer[bottomHalfPos] = '\0';
				//printf("%d\r\n",received);
				displayBuffer();
			}
		}
	}

	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
}

int main(void)
{
    Sys_Init();
    configureSPI();

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);

    HAL_NVIC_EnableIRQ(USART1_IRQn);

    HAL_UART_Receive_IT(&USB_UART, &input, 1);

    displayBuffer();

    while (1)
    {
    	if(closegate == 0){
    		if(rec == 1)
    		{
    			send_character(input);
    			rec = 0;
    		}


    		txData = 0x00U;
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_RESET);
    		if(HAL_SPI_TransmitReceive(&hspi2, &txData, &rxData, 1, HAL_MAX_DELAY) == HAL_OK)
    		{
    			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
    			rxData &= 0xFFU;
    			//uint8_t status = rxData >> 8;
    			rxData &= 0x6U;
    			if(rxData != 0x00U)
    			{
    				receive_character();
    			}
    		}
    		else
    		{
    			HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, GPIO_PIN_SET);
    		}
    	}
    	//HAL_Delay(1000);
    }

}

void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(&USB_UART);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		//HAL_UART_Transmit_IT(&MEET_UART, &input, 1);
		if (closegate == 0)
		{
			if (input == '\033') {
				menu();
			}
			else
			{
				//send_character(input);
				rec = 1;
				//receive_character();
				//displayBuffer();
				//HAL_UART_Receive_IT(&USB_UART, &input, 1);
			}
			HAL_UART_Receive_IT(&USB_UART, &input, 1);
		}
		else
		{
			if (input == '1')
			{
				firmware();
			}
			else if (input == '2')
			{
				get_temperature();
			}
			else if (input == '3')
			{
				reset_peripheral_terminal();
			}
			else if (input == '4')
			{
				rw_id();
			}
			else if (input == '5')
			{
				displayBuffer();
			}
			else
			{

			}
			HAL_UART_Receive_IT(&USB_UART, &input, 1);
		}
	}
}
