/**
  ******************************************************************************
  * @file    stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External variables ---------------------------------------------------------*/
extern volatile uint32_t timingDelay;
extern volatile uint8_t userButtonPressed_F;
extern volatile uint8_t rsetButtonPressed_F;
extern volatile uint8_t newInputData_F;
extern volatile int8_t inputData;

extern volatile changeT changeT_M;
extern volatile progStat status_M;
extern volatile uint8_t wrkCounter;

/* Private variables ---------------------------------------------------------*/
uint8_t bounceFiltered_F = 1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles "START" button pushing
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{ 
	//Test on EXTI transfer complete interrupt
	if (EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != RESET);
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line0);

		if (bounceFiltered_F && (status_M == INI || status_M == FIN)) {
			wrkCounter = 0;	// reinit working counters
			status_M = WRK;
			BBFilter(&bounceFiltered_F);
		}
	}
}

/**
  * @brief  This function handles "+" button pushing
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
	//Test on EXTI transfer complete interrupt
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) != RESET);
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line1);

		if (bounceFiltered_F && status_M == INI) {
			if (GPIOA->IDR & GPIO_IDR_3) {
				changeT_M = INCR_SHOOT;
			} else {
				changeT_M = INCR_WAIT;
			}
			BBFilter(&bounceFiltered_F);
		}
	}
}

/**
  * @brief  This function handles "-" button pushing
  * @param  None
  * @retval None
  */
void EXTI2_TS_IRQHandler(void)
{
	//Test on EXTI transfer complete interrupt
	if (EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) != RESET);
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line2);

		if (bounceFiltered_F && status_M == INI) {
			if (GPIOA->IDR & GPIO_IDR_3) {
				changeT_M = DECR_SHOOT;
			} else {
				changeT_M = DECR_WAIT;
			}
			BBFilter(&bounceFiltered_F);
		}
	}
}

/**
  * @brief  This function handles "RESET" button pushing
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
	//Test on EXTI transfer complete interrupt
	if (EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) != RESET);
		// Clear the EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line4);

		if (bounceFiltered_F && status_M == FIN) {
//			status_M = INI;
			rsetButtonPressed_F = 1;
			BBFilter(&bounceFiltered_F);
		}
	}
}

/**
  * @brief  This function handles Rising edges on Input BUS
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	//Test on EXTI transfer complete interrupt
	if (((EXTI_GetITStatus(EXTI_Line7) == SET) ||
		(EXTI_GetITStatus(EXTI_Line8) == SET) ||
		(EXTI_GetITStatus(EXTI_Line9) == SET)) &&
		(status_M == WRK)) {
		if (!newInputData_F) {
			inputData = InputBus_CaptureState();
			newInputData_F = 1;
//			OutputBus_Clear();
		}
	}
	if (EXTI_GetITStatus(EXTI_Line7) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line7);
	} else if (EXTI_GetITStatus(EXTI_Line8) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line8);
	} else if (EXTI_GetITStatus(EXTI_Line9) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

/*!
  * @brief  This function handles Button bounce.
  * @param  Nope
  * @retval Nope
  */
void TIM6_DAC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update)) {
		TIM_Cmd(TIM6, DISABLE);
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		bounceFiltered_F = 1;
	}
}

/*!
  * @brief  This function handles main TIM IT
  * @param  Nope
  * @retval Nope
  */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
		TIM2->CR1 &= ~TIM_CR1_CEN;
		if (status_M == WRK) {
			wrkCounter++;
			GPIOE->ODR ^= GPIO_ODR_8;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

/*!
  * @brief  This function handles USART1_Tx DMA Interrupt
  * @param  Nope
  * @retval Nope
  */
void DMA1_Channel4_IRQHandler (void)
{
	if (DMA_GetITStatus(DMA1_IT_TC4)) {
		DMA_ClearITPendingBit(DMA1_IT_TC4);
//		GPIOE->ODR ^= GPIO_ODR_8;
		DMA_Cmd(DMA1_Channel4, DISABLE);
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

