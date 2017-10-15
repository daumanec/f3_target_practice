#ifndef _MAIN_H
#define _MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_it.h"

#include "button.h"
#include "f3_usart_zlib.h"
#include "f3_adc_zlib.h"

#include "tests.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	INC_DEC_IDLE_STATE = 0,
	INCR_SHOOT,
	DECR_SHOOT,
	INCR_WAIT,
	DECR_WAIT,
} changeT;

typedef enum {
	INI = 1,
	WRK,
	FIN
} progStat;

/* Public define ------------------------------------------------------------*/
#define DBG

#define WRK_STATES_NUM	4

#define ATTEMPTS_MAX	2

/* Public macro -------------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
//volatile changeT changeT_M;
//volatile progStat status_M;
//volatile uint8_t wrkCounter = 0;

/* Public function prototypes -----------------------------------------------*/
void TimingDelay_Decrement(void);
void BBFilter(uint8_t* status);
void Delay(volatile uint32_t nTime);
int8_t InputBus_CaptureState(void);
void OutputBus_Clear(void);
/* Public functions ---------------------------------------------------------*/

#endif
