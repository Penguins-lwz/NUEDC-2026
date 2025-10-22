#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

char     guart1_TxMsg[UART_BUF_SIZE];
char     guart1_RxMsg[UART_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

enum {
	LOCK,
	UNLOCK,
	UNLOCK_R = 'R',
	UNLOCK_G = 'G',
	UNLOCK_B = 'B',
} __IO status = LOCK;

__IO uint32_t toggleThreshold = 0;

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
static void DebugProcess(uint16_t strLen);

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	LCD_Init(&blcd, &LCD_Font_1608, NULL, WHITE, BLACK);
	LCD_Print(&blcd, 0, 0, "LOCK    ");
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim17);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (status == LOCK && (keyStatus & KEY_FLAG_TAP))
	{
		status = UNLOCK;
		LCD_Print(&blcd, 0, 0, "UNLOCK  ");
	}
	
	else if (status != LOCK && (keyStatus & KEY_FLAG_HOLD))
	{
		status = LOCK;
		LCD_Print(&blcd, 0, 0, "LOCK    ");
	}
	
	static uint32_t cnt = 0;
	if ((cnt += 10) > toggleThreshold * 2) cnt = 0;
	IO_WR(LED_R, status != UNLOCK_R || cnt >= toggleThreshold);
	IO_WR(LED_G, status != UNLOCK_G || cnt >= toggleThreshold);
	IO_WR(LED_B, status != UNLOCK_B || cnt >= toggleThreshold);
}

static void DebugProcess(uint16_t strLen)
{
	sscanf(buart.pRxMsg, "%c:%u", &status, &toggleThreshold);
	strcpy(buart.pRxMsg + strLen - 1, "      ");
	LCD_Print(&blcd, 0, 0, "%s", buart.pRxMsg);
	BSP_UART_Transmit_DMA(&buart, "ok\n");
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart == buart.huart)
	{
		if (status != LOCK) DebugProcess(Size);
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
