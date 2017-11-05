#include "f3_adc_zlib.h"

////-----------------------------------------------------------------------------
////	Init and enable DMA1 Channel1 in circular mode
////	with transfer complete on ADC1 interrupt
////-----------------------------------------------------------------------------
void ADC1_DMA_Init (uint16_t *adc1_dmaField)
{
	DMA_InitTypeDef DMA_InitStructure;

	//Clock DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// Configure structure with factory settings
	DMA_DeInit(DMA1_Channel1);
	// Set the peripheral register address in the DMA_CPARx register
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	// Set the memory address in the DMA_CMARx register
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc1_dmaField;
	// Configure data transfer direction in the DMA_CCRx register
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// Configure the total number of data to be transferred in the DMA_CNDTRx
	DMA_InitStructure.DMA_BufferSize = 2;
	// Configure peripheral & memory incremented mode in the DMA_CCRx register
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// Configure peripheral & memory data size in the DMA_CCRx register
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// Configure circular mode in the DMA_CCRx register
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// Configure the channel priority using the PL[1:0] bits in the DMA_CCRx register
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	// Disable DMA_memory_to_memory in the DMA_CCRx register
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	// Enable DMA1 Channel1 Transfer Complete interrupt
//	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

	// Activate the channel by setting the ENABLE bit in the DMA_CCRx register
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

////-----------------------------------------------------------------------------
////	NVIC for DMA1 Channel1
////-----------------------------------------------------------------------------
//void NVIC_DMA1_Init (void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	// Enable DMA1 channel IRQ Channel
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

/**
  * @brief  ADC continuous conversion mode measurement configuration
  * @note   ADC1 Channel18
  * 		VRefInt
  * @param  None
  * @retval None
  */
uint32_t ADC1_Ini(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	uint32_t calibration_value = 0;

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER |= GPIO_MODER_MODER1;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR1;

	// Configure clock
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div256);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	// Insert delay >10 탎 (72 ticks is 1탎, so wait 1000ticks)
	uint16_t i;
	for (i = 0; i < ((uint32_t)SystemCoreClock*15/1000000); i++){
		asm("nop");
	}

	// Calibration procedure
	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) != RESET );
	calibration_value = ADC_GetCalibrationValue(ADC1);

	// ADC common control register (CCR)
	// Dual ADC mode selection (ADCx_CCR->DUAL)
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
//	// ADC clock mode (ADCx_CCR->CKMODE)
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;
//	// Direct memory access mode for dual ADC mode (ADCx_CCR->MDMA)
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
//	// ADCx_CCR->DMACFG (select between two DMA modes of operation (for dual ADC mode))
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;
//	// ADCx_CCR->DELAY (Delay between 2 sampling phases)
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);

	// Enable the Vrefint channel
//	ADC_VrefintCmd(ADC1, ENABLE);
//	ADC1_2->CCR |= ADC12_CCR_VREFEN;
//	ADC1_2->CCR |= ADC12_CCR_TSEN;
	Delay(1);
	// ADC configuration register & regular sequence register
	// Initialize ADC structure with default settings (they are right one for this task)
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	// Enables DMA in circular mode
//	ADC1->CFGR |= (uint32_t)0x00000003;
	// Configurate regular sequence register (ADCx_SQR) & Sample time selection (ADCx_SMPRx)
	// Total conversion time is calculated as follows: Tconv = Sampling time + 12.5 cycles
	// ADC1 internal channel 18 configuration
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_18, 1, ADC_SampleTime_4Cycles5);	// vref
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_4Cycles5);
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_4Cycles5);	// tsen
	// Enable ADC1
	ADC_Cmd(ADC1, ENABLE);
	/* wait for ADRDY */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
	/* Start ADC1 Software Conversion */
	ADC_StartConversion(ADC1);

	return calibration_value;
}

/**
  * @brief  Start ADC1 regular conversion
  * @note
  * @param  None
  * @retval None
  */
void ADC1_StartRegularConversion(void)
{
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
 	ADC_StartConversion(ADC1);
}

/**
  * @brief  ADC injected measurement configuration
  * @note   ADC1 Channel18
  * 		VRefInt
  * @param  None
  * @retval None
  */
uint32_t ADC1_J_Ini(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InjectedInitTypeDef ADC_Inj_InitStruct;
	uint32_t  calibration_value = 0;

	// Configure the ADC clock (RCC_CFGR2->ADC12PRES)
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div256);
	// Enable ADC1 clock (RCC_AHBENR->ADC12EN)
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	// Enable voltage regulator
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	// Insert delay >10 탎 (72 ticks is 1탎, so wait 1000ticks)
	uint16_t i;
	for (i = 0; i < 1000; i++);

	// Calibration procedure
	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) != RESET );
	calibration_value = ADC_GetCalibrationValue(ADC1);

	// Enable the Vrefint channel
	ADC_VrefintCmd(ADC1, ENABLE);
	// ADC configuration register & regular sequence register
	// Initialize ADC structure with default settings (they are right one for this task)
	ADC_StructInit(&ADC_InitStructure);
	ADC_Init(ADC1, &ADC_InitStructure);
	// Total conversion time is calculated as follows: Tconv = Sampling time + 12.5 cycles
	ADC_InjectedStructInit(&ADC_Inj_InitStruct);
	ADC_Inj_InitStruct.ADC_InjecSequence1 = ADC_InjectedChannel_18;
	ADC_InjectedInit(ADC1, &ADC_Inj_InitStruct);
	ADC_InjectedChannelSampleTimeConfig(ADC1, ADC_InjectedChannel_18, ADC_SampleTime_1Cycles5);
//	NVIC_EnableIRQ(ADC_IRQn);	// don't forget about handler
//	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	return calibration_value;
}

/**
  * @brief  Get voltage from "chs" ADC injected channels
  * @note   Channels should be in consecutive order (1,2,3,   not 1,3,4)
  *			3300 - full ADC scale voltage
  * 		0xFFF = 4095 - 12-bit
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral
  * 		chs: injected channels amount
  * 		adcV: output voltage array
  * @retval Nope
  */
void ADC_J_Get12bitV(ADC_TypeDef* ADCx, uint8_t chs, float *adcV)
{
	uint8_t i;
	uint16_t adcData;

	for (i = 0; i < chs; i++) {
		adcData = ADC_GetInjectedConversionValue(ADCx, ADC_InjectedChannel_1 + i*(0x04));
		*(adcV+i) = (float)(adcData *3000) / 0xFFF / 1000;	// V
	}

}

/**
  * @brief  This function Configure the ADC peripheral
  * @note   ADC1 Channel5
  * 		PF4 pin
  * 		Regular measurement configuration
  * @param  Nope
  * @retval Nope
  */
uint32_t ADC_Ini_cmsisLib(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	uint32_t  calibration_value = 0;

	/* Configure the ADC clock */
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div256);
	/* Enable ADC1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	/* ADC Channel configuration */
	/* GPIOF Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	/* Configure ADC1 Channel5 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	ADC_StructInit(&ADC_InitStructure);
	/* Calibration procedure */
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	/* Insert delay equal to 10 ms */
	Delay(1);

	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) != RESET );
	calibration_value = ADC_GetCalibrationValue(ADC1);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;
//	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
//	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channel7 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_7Cycles5);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	/* wait for ADRDY */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
	/* Start ADC1 Software Conversion */
	ADC_StartConversion(ADC1);

	return calibration_value;
}

uint16_t ADC_GetV_cmsisLib()
{
	uint16_t  ADC1ConvertedValue = 0;
	/* Test EOC flag */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	/* Get ADC1 converted data */
	ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
	/* Compute the voltage */
	return (uint16_t)(((uint32_t)ADC1ConvertedValue *3300)/0xFFF);
}
