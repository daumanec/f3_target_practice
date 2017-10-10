/**
  ******************************************************************************
  * @file    button.h
  * @author  daumanec
  * @version V1.0.0
  * @date    01-February-2017
  * @brief   Functions to simplify button initialization and use
  * 		 implemented for STM32F4-Discovery
  ******************************************************************************
  */

#ifndef _BUTTON_H
#define _BUTTON_H


#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_syscfg.h"
#include "stm32f30x_exti.h"



// ----- Public constants -----------------------------------------------
typedef enum
{
  BUTTON_USER = 0,
  BUTTON_INCR,
  BUTTON_DECR,
  BUTTON_RSET,
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

#define BUTTONn                          4

#define USER_BUTTON_PIN                GPIO_Pin_0
#define USER_BUTTON_GPIO_PORT          GPIOA
#define USER_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define USER_BUTTON_EXTI_LINE          EXTI_Line0
#define USER_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource0
#define USER_BUTTON_EXTI_IRQn          EXTI0_IRQn

#define INCR_BUTTON_PIN                GPIO_Pin_1
#define INCR_BUTTON_GPIO_PORT          GPIOA
#define INCR_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define INCR_BUTTON_EXTI_LINE          EXTI_Line1
#define INCR_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define INCR_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource1
#define INCR_BUTTON_EXTI_IRQn          EXTI1_IRQn

#define DECR_BUTTON_PIN                GPIO_Pin_2
#define DECR_BUTTON_GPIO_PORT          GPIOA
#define DECR_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define DECR_BUTTON_EXTI_LINE          EXTI_Line2
#define DECR_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define DECR_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource2
#define DECR_BUTTON_EXTI_IRQn          EXTI2_TS_IRQn

#define RSET_BUTTON_PIN                GPIO_Pin_4
#define RSET_BUTTON_GPIO_PORT          GPIOA
#define RSET_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define RSET_BUTTON_EXTI_LINE          EXTI_Line4
#define RSET_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define RSET_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource4
#define RSET_BUTTON_EXTI_IRQn          EXTI4_IRQn

// ----- Public functions -----------------------------------------------
void Button_Ini(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t Button_GetState(Button_TypeDef Button);


#endif
