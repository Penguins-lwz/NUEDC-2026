#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

char     guart1_TxMsg[UART_BUF_SIZE];
char     guart1_RxMsg[UART_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

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
	
	LCD_Init(&blcd, &LCD_Font_1608, &LCD_Font_1616, WHITE, BLACK);
	
	LCD_Print(&blcd, 0, 16 * 0, "　　华中科技大学　　");
	LCD_Print(&blcd, 0, 16 * 1, "电工电子科技创新中心");
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, ORANGE,  BLACK);
	LCD_Print(&blcd, 0, 16 * 2, " STM32G474VET6 Demo ");
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, AQUA,    BLACK);
	LCD_Print(&blcd, 0, 16 * 3, "  Unique device ID  ");
	LCD_Print(&blcd, 0, 16 * 4, "      %08X      ", HAL_GetUIDw2());
	LCD_Print(&blcd, 0, 16 * 5, "      %08X      ", HAL_GetUIDw1());
	LCD_Print(&blcd, 0, 16 * 6, "      %08X      ", HAL_GetUIDw0());
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, FUCHSIA, BLACK);
	LCD_Print(&blcd, 0, 16 * 7, "    Penguins-lwz    ");
	
	BSP_UART_Transmit_DMA(&buart, "UIDw2: 0x%08X\n", HAL_GetUIDw2());
	BSP_UART_Transmit_DMA(&buart, "UIDw1: 0x%08X\n", HAL_GetUIDw1());
	BSP_UART_Transmit_DMA(&buart, "UIDw0: 0x%08X\n", HAL_GetUIDw0());
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim17);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	switch (keyStatus)
	{
		case KEY_S1_TAP : LED_ON(R);                       break;
		case KEY_S1_HOLD: LED_OF(R);                       break;
		case KEY_S2_TAP :            LED_ON(G);            break;
		case KEY_S2_HOLD:            LED_OF(G);            break;
		case KEY_S3_TAP :                       LED_ON(B); break;
		case KEY_S3_HOLD:                       LED_OF(B); break;
		case KEY_S4_TAP : LED_ON(R); LED_ON(G); LED_ON(B); break;
		case KEY_S4_HOLD: LED_OF(R); LED_OF(G); LED_OF(B); break;
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
	if (htim == &htim17)
	{
		PeriodicProcess();
	}
}
