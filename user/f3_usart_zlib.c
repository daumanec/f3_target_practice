#include "f3_usart_zlib.h"

//----------------------------------------------------------------------------------//
/**
  * @brief  Configures USART1
  * @note	Use the simplest - polling
  * 		PC-4 - TX
  * 		PC-5 - RX
  * @param  Nope
  * @retval Nope
  */
void USART1_Poll_Ini (void)
{
	GPIO_InitTypeDef GPIO_Init_USART;
	USART_InitTypeDef USART_InitUser;

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_7);

	GPIO_Init_USART.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init_USART.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init_USART.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_USART.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_USART.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_Init_USART);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitUser.USART_BaudRate = USART1_BAUDRATE;
	USART_InitUser.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitUser);

	USART_Cmd(USART1, ENABLE);
}

//----------------------------------------------------------------------------------//
/**
  * @brief  Configures USART1
  * @note	Use IT Handler only on TX channel
  * 		PC-4 - TX
  * 		PC-5 - RX
  * @param  Nope
  * @retval Nope
  */
void USART1_IT_Ini (void)
{
	GPIO_InitTypeDef GPIO_Init_USART;
	USART_InitTypeDef USART_InitUser;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_7);

	GPIO_Init_USART.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init_USART.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init_USART.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_USART.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_USART.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_Init_USART);

	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitUser.USART_BaudRate = USART1_BAUDRATE;
	USART_InitUser.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitUser);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

//----------------------------------------------------------------------------------//
/**
  * @brief  Configures USART1
  * @note	Transmitting using DMA only on Tx Channel
  * 		PC-4 - Tx
  * 		PC-5 - Rx
  * @param  Nope
  * @retval Nope
  */
void USART1_DMA_Ini (uint8_t *buff)
{
	GPIO_InitTypeDef GPIO_Init_USART;
	USART_InitTypeDef USART_InitUser;

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_7);

	GPIO_Init_USART.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init_USART.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init_USART.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_USART.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_USART.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_Init_USART);

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	USART_InitUser.USART_BaudRate = USART1_BAUDRATE;
	USART_InitUser.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitUser);
	USART1->CR2 &= ~USART_CR2_SWAP;
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

//	USART1->BRR = USART1_BAUDRATE;	// here is not baud rate!
									// should be uart clock devider
									// how to calc: RM0316 p.907
//	USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;
//	USART1->CR3 |= USART_CR3_DMAT;

	DMA_USART1Tx_Ini(buff);

	USART_Cmd(USART1, ENABLE);
}

//----------------------------------------------------------------------------------//
// USART3 on GPIOD
//		PD-8 - TX
//		PD-9 - RX
//------------------------------------------------
void USART3_GPIOD_Ini(){
  GPIO_InitTypeDef GPIO_Init_USART;
  USART_InitTypeDef USART_InitUser;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  
  GPIO_Init_USART.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init_USART.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init_USART.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_USART.GPIO_OType = GPIO_OType_PP;
  GPIO_Init_USART.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_Init(GPIOD, &GPIO_Init_USART);
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_7);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  USART_InitUser.USART_BaudRate = 9600;
  USART_InitUser.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitUser.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitUser.USART_Parity = USART_Parity_No;
  USART_InitUser.USART_StopBits = USART_StopBits_1;
  USART_InitUser.USART_WordLength = USART_WordLength_8b;
  
  USART_Init(USART3, &USART_InitUser);
  
  USART_Cmd(USART3, ENABLE);
}

//----------------------------------------------------------------------------------//
/*
 * @brief	USART2 DMA Rx Channel initialization
 *
 */
void USART2_DMARx_Ini (uint8_t* memAdd)
{
	// (1)	Configure structure with factory settings
	// (2)	Set the peripheral register address in the DMA_CPARx register
	// (3)	Set the memory address in the DMA_CMARx register
	// (4)	Configure data transfer direction in the DMA_CCRx register
	// (5)	Configure the total number of data to be transferred in the DMA_CNDTRx
	// (6)	Configure peripheral & memory incremented mode in the DMA_CCRx register
	// (7)	Configure normal mode (not circular) in the DMA_CCRx register
	// (8)	Configure the channel priority using the PL[1:0] bits in the DMA_CCRx register
	// (9)	Disable DMA_memory_to_memory in the DMA_CCRx register
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel6);	// (1)
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->RDR;	// (2)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)memAdd;	// (3)
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	// (4)
	DMA_InitStructure.DMA_BufferSize = RX_PACKET_MAX_LENGTH;	// (5)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	// (6)
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	// (7)
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	// (8)
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	// (9)
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);

	// Enable DMA1 Channel1 Transfer Complete interrupt
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

//	DMA_Cmd(DMA1_Channel6, ENABLE);
}

//----------------------------------------------------------------------------------//
void DMA_USART1Tx_Ini (uint8_t* txAdd)
{
	DMA_InitTypeDef DMA_InitStructure;

	NVIC_SetPriority(DMA1_Channel4_IRQn, 10);
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	// (1)	Configure structure with factory settings
	// (2)	Set the peripheral register address in the DMA_CPARx register
	// (3)	Set the memory address in the DMA_CMARx register
	// (4)	Configure data transfer direction in the DMA_CCRx register
	// (5)	Configure the total number of data to be transferred in the DMA_CNDTRx
	// (6)	Configure peripheral & memory incremented mode in the DMA_CCRx register
	// (7)	Configure normal mode (not circular) in the DMA_CCRx register
	// (8)	Configure the channel priority using the PL[1:0] bits in the DMA_CCRx register
	// (9)	Disable DMA_memory_to_memory in the DMA_CCRx register
	DMA_DeInit(DMA1_Channel4);	// (1)
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->TDR;	// (2)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txAdd;	// (3)
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	// (4)
	DMA_InitStructure.DMA_BufferSize = 0;	// (5)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	// (6)
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	// (7)
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	// (8)
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	// (9)
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

//	DMA_Cmd(DMA1_Channel4, ENABLE);
}


//-------------------------------------------------------------------------//
void PutString_USART1(uint8_t *s)
{
	while (*s != 0)
		if (USART_GetFlagStatus(USART1, USART_FLAG_TXE))
			USART_SendData(USART1, (uint16_t)*s++);
}

//-------------------------------------------------------------------------//
void PutString_DMA_USART1 (const char *s, char* tx)
{
	while (DMA1_Channel4->CCR & DMA_CCR_EN);	// check, that dma is ready
	strcpy(tx, s);
	DMA_SetCurrDataCounter(DMA1_Channel4, strlen(tx));
	DMA1->IFCR &= ~(DMA_IFCR_CGIF4 | DMA_IFCR_CHTIF4 |
				   DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}
