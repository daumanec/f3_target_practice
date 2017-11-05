#include "lcd.h"


/* Private typedef -----------------------------------------------------------*/
#define     LCM_OUT               GPIOD->ODR
#define     LCM_PIN_RS            GPIO_Pin_0          // PD0
#define     LCM_PIN_EN            GPIO_Pin_1          // PD1
#define     LCM_PIN_D4            GPIO_Pin_4	      // PD4
#define     LCM_PIN_D5            GPIO_Pin_5          // PD5
#define     LCM_PIN_D6            GPIO_Pin_6          // PD6
#define     LCM_PIN_D7            GPIO_Pin_7          // PD7
#define     LCM_PIN_MASK  ((LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4))
GPIO_InitTypeDef  GPIO_InitStructure;

/* Privat functions -----------------------------------------------*/
//---Функция задержки---//
void delay(int a)
{
    int i = 0;
    int f = 0;
    while(f<a)
    {
        while(i<600)
        	{i++;}
        f++;
    }
}

//---Нужная функция для работы с дисплее, по сути дергаем ножкой EN---//
void PulseLCD()
{
    LCM_OUT &= ~LCM_PIN_EN;
    delay(220);
    LCM_OUT |= LCM_PIN_EN;
    delay(220);
    LCM_OUT &= (~LCM_PIN_EN);
    delay(220);
}

//---Отсылка байта в дисплей---//
void SendByte(char ByteToSend, int IsData)
{
    LCM_OUT &= (~LCM_PIN_MASK);
    LCM_OUT |= (ByteToSend & 0xF0);

    if (IsData == 1)
        LCM_OUT |= LCM_PIN_RS;
    else
        LCM_OUT &= ~LCM_PIN_RS;
    PulseLCD();
    LCM_OUT &= (~LCM_PIN_MASK);
    LCM_OUT |= ((ByteToSend & 0x0F) << 4);

    if (IsData == 1)
        LCM_OUT |= LCM_PIN_RS;
    else
        LCM_OUT &= ~LCM_PIN_RS;

    PulseLCD();
}


/* Public functions -----------------------------------------------*/
//---Установка позиции курсора---//
void Cursor(char Row, char Col)
{
    char address;
    if (Row == 0)
        address = 0;
    else
        address = 0x40;
    address |= Col;
    SendByte(0x80 | address, 0);
}

//---Очистка дисплея---//
void ClearLCDScreen()
{
    SendByte(0x01, 0);
    SendByte(0x02, 0);
}

int8_t ClearString(uint8_t rowN)
{
	if (rowN != 1 && rowN != 2) {
		return -1;
	}
	if (rowN == 1) {
		SendByte(0x01, 0);
	} else {
		SendByte(0x02, 0);
	}
	return 0;
}

//---Инициализация дисплея---//
void InitializeLCD(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    LCM_OUT &= ~(LCM_PIN_MASK);
    delay(32000);
    delay(32000);
    delay(32000);
    LCM_OUT &= ~LCM_PIN_RS;
    LCM_OUT &= ~LCM_PIN_EN;
    LCM_OUT = 0x20;
    PulseLCD();
    SendByte(0x28, 0);
    SendByte(0x0E, 0);
    SendByte(0x06, 0);
}

//---Печать строки---//
void PrintStr(char *Text)
{
    char *c;
    c = Text;
    while ((c != 0) && (*c != 0))
    {
        SendByte(*c, 1);
        c++;
    }
}
