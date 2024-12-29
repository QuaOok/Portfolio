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

char topHalfBuffer[BUFFER_SIZE] = {0};
char bottomHalfBuffer[BUFFER_SIZE] = {0};
int topHalfPos = 0;
int bottomHalfPos = 0;

// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
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
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; //To operate 1M Hz (13.5 / 16 == 0.85)
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;

	HAL_SPI_Init(&hspi2);
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


		// Initialize CS Pin (PI3)
		GPIO_InitStruct_CS.Pin       = GPIO_PIN_3;
		GPIO_InitStruct_CS.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_CS.Pull      = NO_PULL;
		GPIO_InitStruct_CS.Speed     = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct_CS);

		// Initialize SCK Pin (PA12)
		GPIO_InitStruct.Pin       = GPIO_PIN_12;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Initialize  COPI Pin (PB15)
		GPIO_InitStruct.Pin       = GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// Initialize CIPO Pin (PB14)
		GPIO_InitStruct.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0); // Set the interrupt priority

		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
}

void displayBuffer(void)
{
	printf("\033[2J\033[;H");
	fflush(stdout);

	printf("Your code here!\r\n");
	printf("\033[2;0H--------------------------------------------------------------------------------");
	printf("\033[10;0H--------------------------------------------------------------------------------");
	printf("\033[18;0H--------------------------------------------------------------------------------");

	// Display top half
	printf("\033[3;1H"); // Move to top-left corner
	printf("Top half (Sent):\n%s\n", topHalfBuffer);

	// Display bottom half
	printf("\033[11;1H"); // Move to the bottom half of the terminal
	printf("Bottom half (Received from SPI):\n%s\n", bottomHalfBuffer);
}

int main(void)
{
	Sys_Init();

	// For convenience
	configureSPI();

	uint8_t txData;
	uint8_t rxData;

// See 769 Description of HAL drivers.pdf, Ch. 64.2 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
	while(1)
	{

		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET);

		txData= getchar();

		if(HAL_SPI_TransmitReceive(&hspi2, &txData, 1, HAL_MAX_DEALY) == HAL_OK)
		{
			if (topHalfPos < BUFFER_SIZE - 1)
			{
				topHalfBuffer[topHalfPos++] = txData;
				topHalfBuffer[topHalfPos] = '\0';
			}
			if (bottomHalfPos < BUFFER_SIZE - 1)
			{
				bottomHalfBuffer[bottomHalfPos++] = rxData;
				bottomHalfBuffer[bottomHalfPos] = '\0'; // Null-terminate the string
			}
		}

		displayBuffer();

		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET);
	}
}
