#ifndef _LCD_H
#define _LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"

/* Export function prototypes -----------------------------------------------*/
void InitializeLCD(void);
void PrintStr(char *Text);
void ClearLCDScreen();
int8_t ClearString(uint8_t rowN);
void Cursor(char Row, char Col);

#endif
