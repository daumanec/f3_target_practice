#ifndef _F3_ADC_ZLIB_H
#define _F3_ADC_ZLIB_H


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f30x_adc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Export function prototypes -----------------------------------------------*/
uint32_t ADC1_Ini(void);
uint32_t ADC1_J_Ini(void);
void ADC1_StartRegularConversion(void);
void ADC_J_Get12bitV(ADC_TypeDef* ADCx, uint8_t chs, float *adcV);

/* Private functions ---------------------------------------------------------*/

#endif
