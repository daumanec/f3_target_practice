/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	BEGIN_SHOOT = 0,
	SHOOT,
	BEGIN_WAIT,
	WAIT,
} wrkState;

/* Private define ------------------------------------------------------------*/
#define DBG

//#define INI		0x01
//#define START	0x02

#define LED_B1 GPIO_Pin_8
#define LED_B2 GPIO_Pin_12
#define LED_R1 GPIO_Pin_9
#define LED_R2 GPIO_Pin_13
#define LED_O1 GPIO_Pin_10
#define LED_O2 GPIO_Pin_14
#define LED_G1 GPIO_Pin_11
#define LED_G2 GPIO_Pin_15

#define BUS_FULL_MASK			0x07
#define BUS_MAX_VALUE_MASK		0x05
#define OUTPUT_BUS_DATA_SHIFT	0x08
#define INPUT_BUS_DATA_SHIFT	0x07

#define SHIFT_TO_RESET_BSRR_PART	0x0F

#define SHOOT_MS_DISCR		250
#define WAIT_MS_DISCR		250
#define SHOOT_MS_DEFAULT	2000
#define WAIT_MS_DEFAULT		1000

#define SHOOT_MS_MAX	4950
#define SHOOT_MS_MIN	300
#define WAIT_MS_MAX		4950
#define WAIT_MS_MIN		300

/* Private macro -------------------------------------------------------------*/
/* Export variables ---------------------------------------------------------*/
volatile uint32_t timingDelay = 0;
volatile uint8_t userButtonPressed_F = 0;
volatile uint8_t rsetButtonPressed_F = 0;
volatile uint8_t newInputData_F = 0;
volatile int8_t inputData = 0;
volatile progStat shoot_F = 1;

volatile changeT changeT_M;
volatile progStat status_M;
volatile uint8_t wrkCounter = 0;

uint16_t shootMs;
uint16_t waitMs;

/* Private variables ---------------------------------------------------------*/
uint32_t error = 0;
uint8_t progStatus = INI;
uint8_t txbuff[TX_PACKET_MAX_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void NVIC_Priority_Config (void);
void LEDs_Ini(void);
void LED_Result_Ini(void);
void LED_BadResult_Ini(void);
void Switcher_Ini(void);
void OutputBus_Ini(void);
int8_t OutputBus_Set(uint8_t command);
//void OutputBus_Clear(void);
void InputBus_Ini(void);
//int8_t InputBus_CaptureState(void);
void Delay(volatile uint32_t nTime);
void ErrorHandler();
void BBFilter_Ini(void);
void TIM10secInt_Ini(void);
void TIM10secInt_Set(uint16_t msT);

// ##############################  MAIN  ###################################
int main(void)
{
	uint8_t cmd;
	char outS[50];
	char tmpC[10];
	uint8_t fin_F = 0;

	SysTick_Config(SystemCoreClock / 100);
	NVIC_Priority_Config();
	LEDs_Ini();

	// USART with DMA
	USART1_DMA_Ini(txbuff);

	// Init periph
	LED_Result_Ini();
	LED_BadResult_Ini();
	BBFilter_Ini();
	Button_Ini(BUTTON_USER, BUTTON_MODE_EXTI);
	Button_Ini(BUTTON_RSET, BUTTON_MODE_EXTI);
	Button_Ini(BUTTON_INCR, BUTTON_MODE_EXTI);
	Button_Ini(BUTTON_DECR, BUTTON_MODE_EXTI);
	Switcher_Ini();
	OutputBus_Ini();
	InputBus_Ini();
	TIM10secInt_Ini();

	// Init values
	status_M = INI;
	shootMs = SHOOT_MS_DEFAULT;
	strcpy(outS, "Default shooting time is: ");
	strcat(outS, itoa(shootMs, tmpC, 10));
	strcat(outS, " ms\r\n");
	PutString_DMA_USART1((const char *)outS, (char *) txbuff);
	waitMs = WAIT_MS_DEFAULT;
	strcpy(outS, "Default waiting time is: ");
	strcat(outS, itoa(waitMs, tmpC, 10));
	strcat(outS, " ms\r\n");
//	Delay(10);
	PutString_DMA_USART1((const char *)outS, (char *) txbuff);

	while (!error) {
		switch (status_M){
		case INI:
			if (changeT_M == INCR_SHOOT) {
				if (shootMs < SHOOT_MS_MAX)
					shootMs += SHOOT_MS_DISCR;
			} else if (changeT_M == DECR_SHOOT) {
				if (shootMs > SHOOT_MS_MIN)
					shootMs -= SHOOT_MS_DISCR;
			} else if (changeT_M == INCR_WAIT) {
				if (waitMs < WAIT_MS_MAX)
					waitMs += WAIT_MS_DISCR;
			} else if (changeT_M == DECR_WAIT) {
				if (waitMs > WAIT_MS_MIN)
					waitMs -= WAIT_MS_DISCR;
			} else {
				continue;
			}
			if (changeT_M == INCR_SHOOT || changeT_M == DECR_SHOOT) {
				strcpy(outS, "Shooting time is now: ");
				strcat(outS, itoa(shootMs, tmpC, 10));
			} else {
				strcpy(outS, "Waiting time is now: ");
				strcat(outS, itoa(waitMs, tmpC, 10));
			}
			strcat(outS, " ms\r\n");
			changeT_M = INC_DEC_IDLE_STATE;
			PutString_DMA_USART1((const char *)outS, (char *) txbuff);
			break;
		case WRK:
			while (wrkCounter < ATTEMPTS_MAX * WRK_STATES_NUM) {
				if (wrkCounter % WRK_STATES_NUM == BEGIN_SHOOT) {
					cmd = 3;
					OutputBus_Set(cmd);
					TIM10secInt_Set(shootMs);
					PutString_DMA_USART1("Shoot!\r\n", (char *) txbuff);
					wrkCounter++;
				} else if (wrkCounter % WRK_STATES_NUM == BEGIN_WAIT) {
					OutputBus_Clear();
					TIM10secInt_Set(waitMs);
					PutString_DMA_USART1("Wait!\r\n", (char *) txbuff);
					wrkCounter++;
				}
			}
			GPIOE->BSRR ^= GPIO_BSRR_BS_11;
			status_M = FIN;
			break;
		case FIN:
			if (!fin_F) {
				PutString_DMA_USART1("Finished!\r\n"
					"To repeat the game press \"START\" button.\r\n"
					"To change timings before new game press \"RESTART\""
					" button.\r\n", (char *) txbuff);
				fin_F = 1;
			} else {
				if (rsetButtonPressed_F) {
					status_M = INI;
					fin_F = 0;
					wrkCounter = 0;
				}
			}
			break;
		default:
			error = 1;
		}
//		uint8_t createV = 8;
//		createV++;
	}

	while(1);
}


/* Private functions ---------------------------------------------------------*/
/*!
 * @brief	All NVIC priority configuration
 * @param	Nope
 * @retval	Nope
 */
void NVIC_Priority_Config (void)
{
	SCB->AIRCR = ((uint32_t)0x05FA0000) | ((uint32_t)0x500);

}

//-------------------------------------------------------------------------//
/*!
 * @brief	Initialize LEDs for program status indication
 * @param	Nope
 * @retval	Nope
 */
void LEDs_Ini(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = LED_B1 | LED_B2 |
								  LED_R1 | LED_R2 |
								  LED_O1 | LED_O2 |
								  LED_G1 | LED_G2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Initialize 10 LEDs for GOOD result indication
 * @param	Nope
 * @retval	Nope
 */
void LED_Result_Ini(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
								  GPIO_Pin_2 | GPIO_Pin_3 |
								  GPIO_Pin_4 | GPIO_Pin_5 |
								  GPIO_Pin_6 | GPIO_Pin_7 |
								  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Initialize 5 LEDs for BAD result indication
 * @note	This LEDs Should be ON after the start
 * @param	Nope
 * @retval	Nope
 */
void LED_BadResult_Ini(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
								  GPIO_Pin_2 | GPIO_Pin_3 |
								  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Control value switcher
 * @note	Switcher to flip between "action time" and "idle time"
 * 			values changing possibility
 * 			If PA3 is high - change "action time"
 * 					   low - change "idle time"
 * @param	Nope
 * @retval	Nope
 */
void Switcher_Ini(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Initialize OUTPUT bus to the targets
 * @param	Nope
 * @retval	Nope
 */
void OutputBus_Ini(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 |
								  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Change OUTPUT bus pins condition according to
 * @param	command: Value, that we want to set throw this bus
 * 					 Can be 0...7
 * @retval	0 - success
 * 			-1 - wrong command
 */
int8_t OutputBus_Set(uint8_t command)
{
	if (command > BUS_MAX_VALUE_MASK) {
		return -1;
	}
	GPIOA->BSRR |= ((uint32_t)command << OUTPUT_BUS_DATA_SHIFT);
	return 0;
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Change OUTPUT bus pins condition according to
 * @param	command: Value, that we want to set throw this bus
 * 					 Can be 0...5 (max 5 targets)
 * @retval	Nope
 */
void OutputBus_Clear(void)
{
	GPIOA->BSRR |= ((uint32_t)BUS_FULL_MASK << OUTPUT_BUS_DATA_SHIFT << SHIFT_TO_RESET_BSRR_PART);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Initialize INPUT bus to the targets
 * @param	Nope
 * @retval	Nope
 */
void InputBus_Ini(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 |
								  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect Button EXTI Line to Button GPIO Pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource7);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);

	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Initialize INPUT bus to the targets
 * @param	Nope
 * @retval	-1 - wrong value
 * 			0...5 - target number
 */
int8_t InputBus_CaptureState(void)
{
	uint16_t tmp = 0;
	tmp = GPIOC->IDR & ((uint16_t)BUS_FULL_MASK << INPUT_BUS_DATA_SHIFT);
	tmp >>= INPUT_BUS_DATA_SHIFT;
	if (tmp > BUS_MAX_VALUE_MASK) {
		return -1;
	} else {
		return (int8_t)tmp;
	}
}

//-------------------------------------------------------------------------//
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(volatile uint32_t nTime)
{
  timingDelay = nTime;

  while(timingDelay != 0);
}

//-------------------------------------------------------------------------//
/*!
 * @brief	Shows error code with blinking red LED
 * @param	Error code
 * @retval	Nope
 */
void ErrorHandler ()
{
	uint8_t tmp;
	while(1) {
		// LEDs error
		if (error & 0x0F) {
			GPIOE->BSRR ^= GPIO_BSRR_BS_9;
			Delay(30);
			GPIOE->BSRR ^= GPIO_BSRR_BR_9;
			Delay(100);
			for (tmp = (error & 0x0F); tmp > 0; tmp--) {
				GPIOE->ODR ^= GPIO_ODR_9;
				Delay(10);
			}
			Delay(200);
		} else if (error & 0xF0) {
			GPIOE->BSRR ^= GPIO_BSRR_BS_9;
			Delay(30);
			GPIOE->BSRR ^= GPIO_BSRR_BR_9;
			Delay(30);
			GPIOE->BSRR ^= GPIO_BSRR_BS_9;
			Delay(30);
			GPIOE->BSRR ^= GPIO_BSRR_BR_9;
			Delay(200);
			if ((error & 0xF0) & 0x10) {
				for (tmp = 0; tmp < 2; tmp++) {
					GPIOE->ODR ^= GPIO_ODR_9;
					Delay(20);
				}
			} else if ((error & 0xF0) & 0x20) {
				for (tmp = 0; tmp < 4; tmp++) {
					GPIOE->ODR ^= GPIO_ODR_9;
					Delay(20);
				}
			} else if ((error & 0xF0) & 0x40) {
				for (tmp = 0; tmp < 6; tmp++) {
					GPIOE->ODR ^= GPIO_ODR_9;
					Delay(20);
				}
			} else if ((error & 0xF0) & 0x80) {
				for (tmp = 0; tmp < 8; tmp++) {
					GPIOE->ODR ^= GPIO_ODR_9;
					Delay(20);
				}
			} else {
				for (tmp = 0; tmp < 10; tmp++) {
					GPIOE->ODR ^= GPIO_ODR_9;
					Delay(20);
				}
			}
			Delay(200);
		}
	}
}

//-------------------------------------------------------------------------//
/*!
  * @brief  Button bounce filtering
  * @note	TIM6 makes an timeout after button pressing
  * 		for button IT handler to avoid false response
  * 		because of contact bounce
  * @param  Nope
  * @retval Nope
  */
void BBFilter_Ini(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_Prescaler =
				  (uint16_t)(((SystemCoreClock / 2) / (uint16_t)10000) - 1);
	TIM_TimeBaseInitStruct.TIM_Period = 3000;	// 300ms
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

//-------------------------------------------------------------------------//
/*!
  * @brief  TIM2 initialization
  * @note	Timer to make regular iterations
  * 		Max interval between iterations = 10s, min = 1 ms
  * @param  msT: interval duration in ms (max 10000!!!)
  * @retval Nope
  */
void TIM10secInt_Ini(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_Prescaler =
				  (uint16_t)(((SystemCoreClock / 2) / (uint16_t)2000) - 1);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*!
  * @brief  Set TIM2 period
  * @note	Timer to make regular iterations
  * 		Max interval between iterations = 10s, min = 1 ms
  * @param  msT: interval duration in ms (max 10000!!!)
  * @retval Nope
  */
void TIM10secInt_Set(uint16_t msT) {
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM2->ARR = (2 * msT) - 1;
	TIM2->CR1 |= TIM_CR1_CEN;
}

/* Extern functions ---------------------------------------------------------*/
/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (timingDelay != 0)
  {
    timingDelay--;
  }
}

/*!
  * @brief  Button bounce filtering ENABLE
  * @note	TIM6 makes an timeout after button pressing
  * 		for button IT handler to avoid false response
  * 		because of contact bounce
  * @param  Nope
  * @retval Nope
  */
void BBFilter(uint8_t* status)
{
	*status = 0;
	TIM_Cmd(TIM6, ENABLE);
}
