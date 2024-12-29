#include "init.h" // Always need init.h, otherwise nothing will work.
#include<stdint.h>
#include<stdlib.h>


uint8_t Buffer8101[10];
uint8_t Buffer8102[10];
uint8_t Buffer81001[100];
uint8_t Buffer81002[100];
uint8_t Buffer810001[1000];
uint8_t Buffer810002[1000];

uint16_t Buffer16101[10];
uint16_t Buffer16102[10];
uint16_t Buffer161001[100];
uint16_t Buffer161002[100];
uint16_t Buffer1610001[1000];
uint16_t Buffer1610002[1000];

uint32_t Buffer32101[10];
uint32_t Buffer32102[10];
uint32_t Buffer321001[100];
uint32_t Buffer321002[100];
uint32_t Buffer3210001[1000];
uint32_t Buffer3210002[1000];



DMA_HandleTypeDef hdma2;

void configureDMA();

int main(void)
{
	Sys_Init();
	configureDMA();

	hdma2.Instance = DMA2_Stream0;
	hdma2.Init.Channel = DMA_CHANNEL_0;
	hdma2.Init.Direction = DMA_MEMORY_TO_MEMORY;
	hdma2.Init.MemInc = DMA_MINC_ENABLE;
	hdma2.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma2.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma2.Init.Mode = DMA_NORMAL;
	hdma2.Init.Priority = DMA_PRIORITY_LOW;
	HAL_DMA_Init(&hdma2);


	for (int i = 0; i < 1000; i++){
		if (i < 10) {
			Buffer8101[i] = 1;
			Buffer16101[i] = 1;
			Buffer32101[i] = 1;
		}
		if (i < 100) {
			Buffer81001[i] = 1;
			Buffer161001[i] = 1;
			Buffer321001[i] = 1;
		}
		Buffer810001[i] = 1;
		Buffer1610001[i] = 1;
		Buffer3210001[i] = 1;
	}

	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("\033[0m\033[44;33m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);
	uint32_t cycles = DWT->CYCCNT; // Store the cycle counter

	DWT->CYCCNT = 0;
	for (int i = 0; i < 10; i++){
		Buffer8102[i] = Buffer8101[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint8 buffer 10: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer8101, (uint32_t)Buffer8102, 10);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint8 buffer 10: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 100; i++){
		Buffer81002[i] = Buffer81001[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint8 buffer 100: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer81001, (uint32_t)Buffer81002, 100);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint8 buffer 100: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 1000; i++){
		Buffer810002[i] = Buffer810001[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint8 buffer 1000: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer810001, (uint32_t)Buffer810002, 1000);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint8 buffer 1000: %lu cycles \r\n\n",cycles);

//	printf("Test %d, %d \r\n", Buffer810001[999], Buffer810002[999]);

	hdma2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	HAL_DMA_Init(&hdma2);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 10; i++){
		Buffer16102[i] = Buffer16101[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint16 buffer 10: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer16101, (uint32_t)Buffer16102, 10);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint16 buffer 10: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 100; i++){
		Buffer161002[i] = Buffer161001[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint16 buffer 100: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer161001, (uint32_t)Buffer161002, 100);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint16 buffer 100: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 1000; i++){
		Buffer1610002[i] = Buffer1610001[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint16 buffer 1000: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer1610001, (uint32_t)Buffer1610002, 1000);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint16 buffer 1000: %lu cycles \r\n\n",cycles);

//	printf("Test %d, %d \r\n", Buffer1610001[999], Buffer1610002[999]);

	hdma2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	HAL_DMA_Init(&hdma2);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 10; i++){
		Buffer32102[i] = Buffer32101[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint32 buffer 10: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer32101, (uint32_t)Buffer32102, 10);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint32 buffer 10: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 100; i++){
		Buffer321002[i] = Buffer321001[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint32 buffer 100: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer321001, (uint32_t)Buffer321002, 100);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint32 buffer 100: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	for (int i = 0; i < 1000; i++){
		Buffer3210002[i] = Buffer3210001[i];
	}
	cycles = DWT->CYCCNT;
	printf("C style of uint32 buffer 1000: %lu cycles \r\n",cycles);

	DWT->CYCCNT = 0;
	HAL_DMA_Start(&hdma2, (uint32_t)Buffer3210001, (uint32_t)Buffer3210002, 1000);
	HAL_DMA_PollForTransfer(&hdma2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT;
	printf("DMA of uint32 buffer 1000: %lu cycles \r\n",cycles);

//	printf("Test %ld, %ld", Buffer32101[999], Buffer32102[999]);

	while(1);
}

void configureDMA()
{

}
