#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define PI					3.14159265358979f
#define VREF				3000.0f

char     guart1_TxMsg[UART_BUF_SIZE];
char     guart1_RxMsg[UART_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

uint32_t sigVolt = 0;

BSP_UART_HandleTypeDef buart = {
	.huart  = &huart1,
	.pTxMsg = guart1_TxMsg,
	.pRxMsg = guart1_RxMsg,
};

LCD_HandleTypeDef blcd = {
	.Instance = &LCD_1_80_inch,
	.hspi     = &hspi4,
	.dir      = LCD_DIR_RIGHT,
	.TxBuf    = glcd_TxBuf,
};

static void PeriodicProcess(void);

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	LCD_Init(&blcd, &LCD_Font_1608, NULL, WHITE, BLACK);
	
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim17);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus)
	{
		if (sigVolt ==    0) sigVolt =  500; else
		if (sigVolt ==  500) sigVolt = 1000; else
		if (sigVolt == 1000) sigVolt = 1500; else
		if (sigVolt == 1500) sigVolt = 2000; else
		if (sigVolt == 2000) sigVolt = 2500; else
		if (sigVolt == 2500) sigVolt =    0;
		
		DAC1->DHR12R1 = sigVolt * 4096 / VREF;
	}
	
	static uint16_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
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
	if (htim == &htim17)
	{
		PeriodicProcess();
	}
}
