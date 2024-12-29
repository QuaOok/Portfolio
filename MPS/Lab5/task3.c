#include "init.h"

void configureADC();
void configureDAC();
void Init_Timer();

DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_dac_ch1;
ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim6;

int convCompletedHalf = 0;
int convCompleted = 0;
uint32_t adc_val[100];
uint32_t dac_val[100];

float sampleValues[3];
float floatValue;
float outValue;
float previousOut = 0;

float w1 = 0.312500;
float w2 = 0.240385;
float w3 = 0.296875;

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	Init_Timer();

	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	HAL_NVIC_EnableIRQ(ADC_IRQn);


	configureADC();
    configureDAC();

	// Code goes here
	HAL_TIM_Base_Start(&htim6);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_val, 100);
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)dac_val, 100, DAC_ALIGN_12B_R);

	while (1) {
	}
}

void configureADC()
{
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T6_TRGO;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	HAL_ADC_Init(&hadc1);


	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	/* ADC1 DMA Init */
	hdma_adc1.Instance = DMA2_Stream0;
	hdma_adc1.Init.Channel = DMA_CHANNEL_0;
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adc1.Init.Mode = DMA_CIRCULAR;
	hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&hdma_adc1);

	__HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &InitStruct);

}

void configureDAC()
{
	// Enable the DAC Clock.
	hdac.Instance = DAC;
	HAL_DAC_Init(&hdac); // Initialize the DAC

	DAC_ChannelConfTypeDef sConfig;

	sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

	/* Peripheral DMA init*/
	hdma_dac_ch1.Instance = DMA1_Stream5;
	hdma_dac_ch1.Init.Channel = DMA_CHANNEL_7;
	hdma_dac_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_dac_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_dac_ch1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_dac_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_dac_ch1.Init.Mode = DMA_CIRCULAR;
	hdma_dac_ch1.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&hdma_dac_ch1);

	__HAL_LINKDMA(&hdac,DMA_Handle1,hdma_dac_ch1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hadc)
{
	// GPIO init
	GPIO_InitTypeDef InitStruct;
	InitStruct.Pin = GPIO_PIN_4;
	InitStruct.Mode = GPIO_MODE_ANALOG;
	InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &InitStruct);
}

void DMA2_Stream0_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_adc1);
}
void DMA1_Stream5_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_dac_ch1);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef*hadc) {
	for (int i = 0; i < 50; i++){
		floatValue = ((float)adc_val[i]) / 4095 * 3300;
		sampleValues[0] = sampleValues[1];
		sampleValues[1] = sampleValues[2];
		sampleValues[2] = floatValue;

		dac_val[i] = w1 * sampleValues[2] + w2 * sampleValues[1] + w1 * sampleValues[0] + w3 * previousOut;
		previousOut = dac_val[i];
//		dac_val[i] = adc_val[i];
	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef*hadc) {
	for (int i = 50; i < 100; i++){
		floatValue = ((float)adc_val[i]) / 4095 * 3300;
		sampleValues[0] = sampleValues[1];
		sampleValues[1] = sampleValues[2];
		sampleValues[2] = floatValue;
//
		dac_val[i] = w1 * sampleValues[2] + w2 * sampleValues[1] + w1 * sampleValues[0] + w3 * previousOut;
		previousOut = dac_val[i];

//		dac_val[i] = adc_val[i];
	}
}

void Init_Timer()
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	//(108 * 10) / 108M Hz == 100 kHz
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 108-1;
	htim6.Init.Period = 10-1;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim6);   // Configure the timer

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
}

