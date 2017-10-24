
#include "tests.h"

/*!
 * @brief	Test GPIO for LEDs redult indication ini
 * @param	Nope
 * @retval	if 0 - success
 * 			if -n - error while checking LED with number n
 * 					(n = 10 is for 0)
 */
int8_t test_ResultLedsIni(void)
{
	GPIOB->BSRR |= GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 |
				   GPIO_BSRR_BS_2 | GPIO_BSRR_BS_3 |
				   GPIO_BSRR_BS_4 | GPIO_BSRR_BS_5 |
				   GPIO_BSRR_BS_6 | GPIO_BSRR_BS_7 |
				   GPIO_BSRR_BS_8 | GPIO_BSRR_BS_9;

	// Start checking. If some GPIO is low - return that number
	if (GPIOB->ODR & GPIO_ODR_0) {
		if (GPIOB->ODR & GPIO_ODR_1) {
			if (GPIOB->ODR & GPIO_ODR_2) {
				if (GPIOB->ODR & GPIO_ODR_3) {
					if (GPIOB->ODR & GPIO_ODR_4) {
						if (GPIOB->ODR & GPIO_ODR_5) {
							if (GPIOB->ODR & GPIO_ODR_6) {
								if (GPIOB->ODR & GPIO_ODR_7) {
									if (GPIOB->ODR & GPIO_ODR_8) {
										if (GPIOB->ODR & GPIO_ODR_9) {
											// Reset all GPIOs to low and go out
											GPIOB->BSRR |= GPIO_BSRR_BR_0 |
														   GPIO_BSRR_BR_1 |
														   GPIO_BSRR_BR_2 |
														   GPIO_BSRR_BR_3 |
														   GPIO_BSRR_BR_4 |
														   GPIO_BSRR_BR_5 |
														   GPIO_BSRR_BR_6 |
														   GPIO_BSRR_BR_7 |
														   GPIO_BSRR_BR_8 |
														   GPIO_BSRR_BR_9;
											return 0;
										}
										return -9;
									}
									return -8;
								}
								return -7;
							}
							return -6;
						}
						return -5;
					}
					return -4;
				}
				return -3;
			}
			return -2;
		}
		return -1;
	}
	return -10;
}

/*!
 * @brief	Test LEDs visualy
 * @note	Turn on all result LEDs for 1sec
 * 			Turning LEDs OFF is provided with function "test_ResultLedsIni()"
 * @param	Nope
 * @retval	Nope
 */
void test_LedsVisualy(void)
{
	GPIOB->BSRR |= GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 |
				   GPIO_BSRR_BS_2 | GPIO_BSRR_BS_3 |
				   GPIO_BSRR_BS_4;
	Delay(30);
	GPIOB->BSRR |= GPIO_BSRR_BR_0 | GPIO_BSRR_BR_1 |
				   GPIO_BSRR_BR_2 | GPIO_BSRR_BR_3 |
				   GPIO_BSRR_BR_4;

	GPIOC->BSRR |= GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 |
				   GPIO_BSRR_BS_2 | GPIO_BSRR_BS_3 |
				   GPIO_BSRR_BS_6;
	Delay(30);
	GPIOC->BSRR |= GPIO_BSRR_BR_0 | GPIO_BSRR_BR_1 |
				   GPIO_BSRR_BR_2 | GPIO_BSRR_BR_3 |
				   GPIO_BSRR_BR_6;
}

/*!
 * @brief	Test Button
 * @note	Wait 5 seconds for the pressing flag, that should be set in the IT handler
 * 			If 5 seconds timeout is over - return error
 * @param	press_F: pointer to the flag, that shows the pressing event
 * @retval	if 0 - success
 * 			if -1 - timeout is over
 * 					(n = 10 is for 0)
 */
int8_t test_Button(uint8_t *press_F)
{
	uint8_t timeOut = 50; // 5 sec timeout
	GPIOE->BSRR |= GPIO_BSRR_BS_10;
	while (!*press_F && timeOut > 0) {
		timeOut --;
		Delay(10);
	}
	GPIOE->BSRR |= GPIO_BSRR_BR_10;
	if (timeOut == 0) {
		return -1;
	}
	*press_F = 0;	// clear the flag
	return 0;
}

/*!
 * @brief	Test Switcher
 * @note	Wait 5 seconds for the switch (EVENT)
 * 			If 5 seconds timeout is over - return error
 * @param	Nope
 * @retval	if 0 - success
 * 			if -1 - timeout is over
 * 					(n = 10 is for 0)
 */
int8_t test_Switch(void)
{
	uint16_t tmp;
	uint8_t timeOut = 50; // 5 sec timeout
	GPIOE->BSRR |= GPIO_BSRR_BS_10;
	tmp = (GPIOA->IDR & GPIO_IDR_3);
	while (tmp == (GPIOA->IDR & GPIO_IDR_3) && timeOut > 0) {
		timeOut --;
		Delay(10);
	}
	GPIOE->BSRR |= GPIO_BSRR_BR_10;
	if (timeOut == 0) {
		return -1;
	}
	return 0;
}


int8_t test_Usart2IT(uint8_t* tx)
{
	int8_t a = 10;
	return a;
}

void test_RandomGen(char *outS, char *tmpC, uint8_t *txbuff)
{
	uint8_t cmd = 0;
	uint16_t  adcConvertedValue1 = 0, adcConvertedValue2 = 0;
	Delay(100);
	while(1) {

		/* Test EOC flag */
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		/* Get ADC1 converted data */
		while (!(ADC1->ISR & ADC_ISR_EOC));
		adcConvertedValue1 = ADC_GetConversionValue(ADC1);
		while (!(ADC1->ISR & ADC_ISR_EOC));
		adcConvertedValue2 = ADC_GetConversionValue(ADC1);

//		cmd = (uint8_t)((adcConvertedValue1 % 5) + 1);
		cmd = (uint8_t)((((uint32_t)adcConvertedValue1*adcConvertedValue2) % 5) + 1);
		strcpy(outS, "Target number is: ");
		strcat(outS, itoa(cmd, tmpC, 10));
		strcat(outS, ", adc codes = ");
		strcat(outS, itoa(adcConvertedValue1, tmpC, 10));
		strcat(outS, ", ");
		strcat(outS, itoa(adcConvertedValue2, tmpC, 10));
		strcat(outS, "\r\n");
		PutString_DMA_USART1((const char *)outS, (char *) txbuff);
		Delay(100);
	}
}
