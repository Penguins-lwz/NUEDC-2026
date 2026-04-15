#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "lcd.h"

char guart1_TxMsg[UART_BUF_SIZE];
char guart1_RxMsg[UART_BUF_SIZE];

const uint32_t varF[6] = { 1,  2,  4,  5,  8,  10 };
const uint32_t varD[6] = { 0, 20, 40, 60, 80, 100 };

uint32_t fa6, fa7, da6, da7;

BSP_UART_HandleTypeDef buart = {
	.huart  = &huart1,
	.pTxMsg = guart1_TxMsg,
	.pRxMsg = guart1_RxMsg,
};

static void LCD_Print(uint8_t Line, const char *format, ...);
static void PeriodicProcess(void);

void MainProcess(void)
{
	LED_WR(0x00);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_Print(Line2, "      PWM TEST      ");
	
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim15);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus & KEY_NUM_S1) fa6++, fa6 %= 6, TIM16->PSC  = 800 / varF[fa6] - 1;
	if (keyStatus & KEY_NUM_S2) fa7++, fa7 %= 6, TIM17->PSC  = 800 / varF[fa7] - 1;
	if (keyStatus & KEY_NUM_S3) da6++, da6 %= 6, TIM16->CCR1 = varD[da6];
	if (keyStatus & KEY_NUM_S4) da7++, da7 %= 6, TIM17->CCR1 = varD[da7];
	
	static uint32_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LCD_Print(Line4, "   PA6:%2ukHz,%3u%%", varF[fa6], varD[da6]);
	LCD_Print(Line5, "   PA7:%2ukHz,%3u%%", varF[fa7], varD[da7]);
	
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
