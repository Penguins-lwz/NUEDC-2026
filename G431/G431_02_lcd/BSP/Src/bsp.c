#include "bsp.h"
#include "bsp_gpio.h"
#include "lcd.h"

static void LCD_Print(uint8_t Line, const char *format, ...);
static void PeriodicProcess(void);

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	LED_WR(0x00);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_Print(Line2, "      KEY TEST      ");
	LCD_Print(Line4, "     CNT: 0         ");
	LCD_Print(Line5, "     KEY: NONE      ");
	LCD_Print(Line6, "    TYPE: NONE      ");
	
	HAL_TIM_Base_Start_IT(&htim15);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	static uint16_t cnt = 0;
	uint8_t keyStatus = KEY_Read();
	if (keyStatus == 0x00) return;
	
	LED_WR(keyStatus);
	LCD_Print(Line4, "     CNT: %d", ++cnt);
	
	switch (keyStatus & 0x0F)
	{
		case 0x01: LCD_Print(Line5, "     KEY: S1        "); break;
		case 0x02: LCD_Print(Line5, "     KEY: S2        "); break;
		case 0x04: LCD_Print(Line5, "     KEY: S3        "); break;
		case 0x08: LCD_Print(Line5, "     KEY: S4        "); break;
		default:   LCD_Print(Line5, "     KEY: MULTI     "); break;
	}
	
	switch (keyStatus & 0xF0)
	{
		case 0x10: LCD_Print(Line6, "    TYPE: TAP       "); break;
		case 0x20: LCD_Print(Line6, "    TYPE: HOLD      "); break;
		case 0x40: LCD_Print(Line6, "    TYPE: DOUBLE    "); break;
		default: break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim15)
	{
		PeriodicProcess();
	}
}

static void LCD_Print(uint8_t Line, const char *format, ...)
{
	static char str[21];
	va_list ap;
	va_start(ap, format);
	int xnum = vsnprintf(str, 21, format, ap);
	va_end(ap);
	while (xnum < 20) str[xnum++] = 32;
	LCD_DisplayStringLine(Line, (uint8_t *)str);
}
