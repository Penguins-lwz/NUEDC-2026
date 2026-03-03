#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_at24c02.h"
#include "lcd.h"

char guart1_TxMsg[UART_BUF_SIZE];
char guart1_RxMsg[UART_BUF_SIZE];

uint8_t pwrCnt;

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
	
	I2CInit();
	pwrCnt = ROM_Read(0x00);
	ROM_Write(0x00, ++pwrCnt);
	LCD_Print(Line2, "    E2PROM TEST     ");
	LCD_Print(Line4, "    CNT: %u", pwrCnt);
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim15);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus)
	{
		ROM_Write(0x00, 0x00);
		LCD_Print(Line4, "    CNT: Cleared    ");
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
