#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "lcd.h"

char guart1_TxMsg[UART_BUF_SIZE];
char guart1_RxMsg[UART_BUF_SIZE];

BSP_UART_HandleTypeDef buart = {
	.huart  = &huart1,
	.pTxMsg = guart1_TxMsg,
	.pRxMsg = guart1_RxMsg,
};

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
	
	BSP_UART_Transmit_DMA(&buart, "UIDw2: 0x%08X\n", HAL_GetUIDw2());
	BSP_UART_Transmit_DMA(&buart, "UIDw1: 0x%08X\n", HAL_GetUIDw1());
	BSP_UART_Transmit_DMA(&buart, "UIDw0: 0x%08X\n", HAL_GetUIDw0());
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart == buart.huart)
	{
		BSP_UART_Transmit_DMA(&buart, "RxLen: %u, RxMsg: %s", Size, buart.pRxMsg);
		BSP_UART_ReceiveToIdle_DMA(&buart);
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
