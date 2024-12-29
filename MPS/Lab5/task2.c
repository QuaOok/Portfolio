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
DMA_HandleTypeDef hdma1_spi2_tx;
DMA_HandleTypeDef hdma1_spi2_rx;

#define BUFFER_SIZE 128
char topHalfBuffer[BUFFER_SIZE] = {0};
char bottomHalfBuffer[BUFFER_SIZE] = {0};
int topHalfPos = 0;
int bottomHalfPos = 0;

uint8_t input;

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


		// Initialize CS Pin (PI3)
		GPIO_InitStruct_CS.Pin       = GPIO_PIN_3;
		GPIO_InitStruct_CS.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct_CS.Pull      = GPIO_NOPULL;
		GPIO_InitStruct_CS.Speed     = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct_CS);

		// Initialize SCK Pin (PA12)
		GPIO_InitStruct.Pin       = GPIO_PIN_12;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// Initialize  COPI Pin (PB15)
		GPIO_InitStruct.Pin       = GPIO_PIN_15;
		//GPIO_InitStruct.Pull      = GPIO_PULLUP;

		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		// Initialize CIPO Pin (PB14)
		GPIO_InitStruct.Pin 	  = GPIO_PIN_14;
		//GPIO_InitStruct.Pull      = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//DMA 1
		//Channel 0 Stream 3 for SPI2_RX
		//Channel 0 Stream 4 for SPI2_TX

		hdma1_spi2_rx.Instance = DMA1_Stream3;
		hdma1_spi2_rx.Init.Channel = DMA_CHANNEL_0;
		hdma1_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma1_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma1_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma1_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma1_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma1_spi2_rx.Init.Mode = DMA_NORMAL;
		hdma1_spi2_rx.Init.Priority = DMA_NORMAL;
		hdma1_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma1_spi2_rx);

		__HAL_LINKDMA(&hspi2,hdmarx,hdma1_spi2_rx);

		hdma1_spi2_tx.Instance = DMA1_Stream4;
		hdma1_spi2_tx.Init.Channel = DMA_CHANNEL_0;
		hdma1_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma1_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma1_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma1_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma1_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma1_spi2_tx.Init.Mode = DMA_NORMAL;
		hdma1_spi2_tx.Init.Priority = DMA_NORMAL;
		hdma1_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma1_spi2_tx);

		__HAL_LINKDMA(&hspi2,hdmatx,hdma1_spi2_tx);

		HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0); // Set the interrupt priority

		HAL_NVIC_EnableIRQ(SPI2_IRQn);

		HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

		HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
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

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);

    HAL_NVIC_EnableIRQ(USART1_IRQn);

    HAL_UART_Receive_IT(&USB_UART, &input, 1);

    displayBuffer();

// See 769 Description of HAL drivers.pdf, Ch. 64.2 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
	while(1)
	{
		//receive_line();
	}
}

void DMA1_Stream3_IRQHandler()
{
    HAL_DMA_IRQHandler(&hdma1_spi2_rx); // Process the DMA RX interrupt
}

void DMA1_Stream4_IRQHandler()
{
    HAL_DMA_IRQHandler(&hdma1_spi2_tx);
}

void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(&USB_UART);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		if(input == '\r' || input == '\n')
		{
			//send_line();
			if(HAL_SPI_TransmitReceive_DMA(&hspi2, &topHalfBuffer, &bottomHalfBuffer, BUFFER_SIZE) != HAL_OK)
			{
				  printf("Error initiating SPI DMA Transmit&Receive\n");
			}
		}
		else
		{
			if (topHalfPos < BUFFER_SIZE - 1)
			{
				topHalfBuffer[topHalfPos++] = input;
				topHalfBuffer[topHalfPos] = '\0';
			}
		}
		//displayBuffer();
		HAL_UART_Receive_IT(&USB_UART, &input, 1);
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	displayBuffer();
}



