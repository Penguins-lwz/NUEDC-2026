#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define PI					3.14159265358979f

#define DAC_BUF_SIZE		100U

char     guart1_TxMsg[UART_BUF_SIZE];
char     guart1_RxMsg[UART_BUF_SIZE];
uint16_t gdac_ConvData[DAC_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

uint32_t sigFreq = 10000;

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
	
	for (uint32_t i = 0; i < DAC_BUF_SIZE; ++i)
		gdac_ConvData[i] = 2048 + 2047 * sinf(2.0f * PI * i / DAC_BUF_SIZE);
	
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)gdac_ConvData, DAC_BUF_SIZE, DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim6);
	
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
		if (sigFreq == 10000) sigFreq =  5000; else
		if (sigFreq ==  5000) sigFreq =  2000; else
		if (sigFreq ==  2000) sigFreq =  1000; else
		if (sigFreq ==  1000) sigFreq =   500; else
		if (sigFreq ==   500) sigFreq =   200; else
		if (sigFreq ==   200) sigFreq =   100; else
		if (sigFreq ==   100) sigFreq = 10000;
		
		TIM6->ARR = 2000000 / sigFreq - 1;
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
