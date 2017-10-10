#ifndef _TESTS_H
#define _TESTS_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f30x.h"
#include "main.h"
#include "f3_usart_zlib.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int8_t test_ResultLedsIni(void);
void test_LedsVisualy(void);
int8_t test_Button(uint8_t *press_F);
int8_t test_Switch(void);
/* Private functions ---------------------------------------------------------*/


#endif
