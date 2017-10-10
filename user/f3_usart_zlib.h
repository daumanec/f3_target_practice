#ifndef _F3_USART_ZLIB_H
#define _F3_USART_ZLIB_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_misc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART1_BAUDRATE			9600
#define RX_PACKET_MAX_LENGTH	10
#define TX_PACKET_MAX_LENGTH	50
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Export function prototypes -----------------------------------------------*/
void USART1_IT_Ini (void);
void USART1_DMA_Ini (uint8_t *buff); // configure usart including dma
void DMA_USART1Tx_Ini (uint8_t* txAdd);	// configure dma
void PutString_USART1(uint8_t *s);
void PutString_DMA_USART1 (const char *s, char* tx);
/* Private functions ---------------------------------------------------------*/

#endif
