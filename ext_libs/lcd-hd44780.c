//************************************************************************//
//	LCD HD44780
//	Alex_EXE 
//	http://alex-exe.ru/category/radio/stm32/stm32-lcd-hd44780-spl
//************************************************************************//
#include "lcd-hd44780.h"

const unsigned char lcd44780_addLUT[4] = {0x80, 0xC0, 0x94, 0xD4};
unsigned char lcd44780_Address, lcd44780_Line;

//************************************************************************//
GPIO_InitTypeDef lcd44780_GPIO_InitStructure;	//	Ñ�Ñ‚Ñ€ÑƒÐºÑ‚ÑƒÑ€Ð° Ð´Ð»Ñ� Ð¸Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ð¸ Ð²Ñ‹Ð²Ð¾Ð´Ð¾Ð² Ð¸Ñ�Ð¿Ð¾Ð»ÑŒÐ·ÑƒÐµÐ¼Ñ‹Ñ… Ð´Ð¸Ñ�Ð¿Ð»ÐµÐµÐ¼

//	Ð·Ð°Ð´ÐµÑ€Ð¶ÐºÐ°
//	Input : p - Ð²ÐµÐ»Ð¸Ñ‡Ð¸Ð½Ð° Ð·Ð°Ð´ÐµÑ€Ð¶ÐºÐ¸
void lcd44780_delay(unsigned int p)
{
	unsigned long i;
	for(i=0;i<(p*10);i++){}
}

//	Ð¸Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ� Ð²Ñ‹Ð²Ð¾Ð´Ð¾Ð² Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ�
void lcd44780_init_pins(void)
{
	RCC_AHBPeriphClockCmd(lcd44780_RCC, ENABLE);
	lcd44780_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	lcd44780_GPIO_InitStructure.GPIO_Pin = 	lcd44780_pin_E | lcd44780_pin_RS | lcd44780_pins_data;
	lcd44780_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	lcd44780_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//	Ð²Ñ‹Ñ…Ð¾Ð´Ñ‹ Ñ� Ð¾Ñ‚ÐºÑ€Ñ‹Ñ‚Ñ‹Ð¼ ÐºÐ¾Ð»Ð»ÐµÐºÑ‚Ð¾Ñ€Ð¾Ð¼, Ð½ÑƒÐ¶Ð½Ð° Ð¿Ð¾Ð´Ð²Ñ�Ð·ÐºÐ° Ðº +5Ð’
	GPIO_Init(lcd44780_port, &lcd44780_GPIO_InitStructure);
}
//************************************************************************//

//	ÐžÑ‚Ð¿Ñ€Ð°Ð²ÐºÐ° Ð¿Ð¾Ð»ÑƒÐ±Ð°Ð¹Ñ‚Ð° Ð´Ð¸Ñ�Ð¿Ð»ÐµÑŽ
//	Input : data - Ð¿Ð¾Ð» Ð±Ð°Ð¹Ñ‚Ð° Ð´Ð°Ð½Ð½Ñ‹Ñ…
void lcd44780_WriteNibble(unsigned char data)
{
	GPIO_SetBits(lcd44780_port, ((data & 0x0F))<<lcd44780_offset);
	lcd44780_delay(200);
	lcd44780_E_1;
	lcd44780_delay(100);
	lcd44780_E_0;
	GPIO_ResetBits(lcd44780_port, 0x0F<<lcd44780_offset);
}

//	ÐžÑ‚Ð¿Ñ€Ð°Ð²ÐºÐ° Ð±Ð°Ð¹Ñ‚Ð° Ð´Ð¸Ñ�Ð¿Ð»ÐµÑŽ
//	Input : data - Ð¾Ñ‚Ð¿Ñ€Ð°Ð²Ð»Ñ�ÐµÐ¼Ñ‹Ðµ Ð´Ð°Ð½Ð½Ñ‹Ðµ
void lcd44780_WriteByte(unsigned char data)
{
	lcd44780_WriteNibble(data >> 4);
	lcd44780_WriteNibble(data & 0x0F);
}

//	ÐŸÐµÑ€ÐµÐ¹Ñ‚Ð¸ Ð½Ð° Ñ�Ñ‚Ñ€Ð¾Ñ‡ÐºÑƒ
//	Input : LineNum - Ð½Ð¾Ð¼ÐµÑ€ Ñ�Ñ‚Ñ€Ð¾ÐºÐ¸
void lcd44780_GoToLine(char LineNum)
{
	lcd44780_RS_0;
	lcd44780_Address = lcd44780_addLUT[LineNum-1];
	lcd44780_WriteByte(lcd44780_Address);
	lcd44780_RS_1;
	lcd44780_Address = 0;
	lcd44780_Line = LineNum;
}

//	ÐžÑ‡Ð¸Ñ�Ñ‚ÐºÐ° Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ�
void lcd44780_ClearLCD(void)
{
	lcd44780_RS_0;
	lcd44780_WriteByte(0x01);
	lcd44780_delay(1000);
	
	// Display on, cursor off, cursor position off
	lcd44780_WriteByte(0x0C);
	lcd44780_delay(1000);
	
	lcd44780_RS_1;
	lcd44780_GoToLine(1);
}

//	Ð£Ñ�Ñ‚Ð°Ð½Ð¾Ð²ÐºÐ° ÐºÑƒÑ€Ñ�Ð¾Ñ€Ð° Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ�
//	Input : x , y - cursor coordinate
//	00	10	20	30	40	50	60	70	80	...
//	01	11	21	31	41	51	61	71	81	...
void lcd44780_SetLCDPosition(char x, char y)
{
	lcd44780_RS_0;
	lcd44780_Address = lcd44780_addLUT[(int)y] + (int)x;
	lcd44780_WriteByte(lcd44780_Address);
	lcd44780_RS_1;
	lcd44780_Line = y+1;
}

//	ÐžÑ‚Ð¿Ñ€Ð°Ð²ÐºÐ° Ñ�Ð¸Ð¼Ð²Ð¾Ð»Ð°
//	Input : c - Ñ�Ð¸Ð¼Ð²Ð¾Ð»
void lcd44780_ShowChar(unsigned char c)
{
	lcd44780_RS_1;
	lcd44780_WriteByte(c);
	lcd44780_Address++;
	switch (lcd44780_Address)
	{
		case 20: lcd44780_GoToLine(2); break;
		case 40: lcd44780_GoToLine(3); break;
		case 60: lcd44780_GoToLine(4); break;
		case 80: lcd44780_GoToLine(1); break;
	}
}

//	ÐžÑ‚Ð¿Ñ€Ð°Ð²ÐºÐ° Ñ�Ñ‚Ñ€Ð¾ÐºÐ¸
//	Input : *s - Ñ�Ñ�Ñ‹Ð»ÐºÐ° Ð½Ð° Ñ�Ñ‚Ñ€Ð¾ÐºÑƒ (Ð¼Ð°Ñ�Ñ�Ð¸Ð² Ñ�Ð¸Ð¼Ð²Ð¾Ð»Ð¾Ð²)
void lcd44780_ShowStr(char *s)
{
	while (*s != 0) lcd44780_ShowChar((unsigned char)*s++);
}

//	Ð˜Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ� Ð´Ð¸Ñ�Ð¿Ð»ÐµÑ�
void lcd44780_init(void)
{
	unsigned char i;
	lcd44780_E_0;
	lcd44780_RS_0;
	lcd44780_delay(5000);
	
	// Init
	lcd44780_WriteNibble(0x30);
	lcd44780_WriteNibble(0x30);
	lcd44780_WriteNibble(0x30);
	
	// Function set
	lcd44780_WriteNibble(0x20);	// interface data is 4 bits
	
	// Function set
	lcd44780_WriteByte(0x28);	//2 lines, font 5x8
	
	// Clear display
	lcd44780_WriteByte(0x01);
	
	// Cursor or display shift
	lcd44780_WriteByte(0x10);	//shift cursor to the left
	
	// Entry mode set
	lcd44780_WriteByte(0x06);	//DDRAM address increases
								//no display shifting
	
	// Display ON/OFF
	lcd44780_WriteByte(0x0C);	//again in lcd44780_ClearLCD,
								//because Clear Display instruction
								//turns the toggling cursor ON
	// Without this data wrong
	for(i=0x40; i<0x5F; i++)
	{
		lcd44780_delay(1000);
		lcd44780_RS_0;
		lcd44780_WriteByte(i);
		lcd44780_delay(1000);
		lcd44780_ShowChar(0);
	}
	lcd44780_RS_1;
	lcd44780_ClearLCD();
}
