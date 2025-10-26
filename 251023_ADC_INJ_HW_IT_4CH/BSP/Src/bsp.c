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
static void SampleProcess(void);

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	LCD_Init(&blcd, &LCD_Font_1608, &LCD_Font_1616, WHITE, BLACK);
	LCD_Print(&blcd, 0, 16 * 0, "INJ_HW_IT_4CH");
	
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_InjectedStart_IT(&hadc1);
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim17);
	HAL_TIM_Base_Start(&htim6);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	static uint16_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
}

static void SampleProcess(void)
{
	static uint16_t timeCnt = 0;
	if (++timeCnt < 2000) return;
	timeCnt = 0;
	
	LCD_Print(&blcd, 0, 16 * 1, "PA0: %04u", ADC1->JDR1);
	LCD_Print(&blcd, 0, 16 * 2, "PA1: %04u", ADC1->JDR2);
	LCD_Print(&blcd, 0, 16 * 3, "PA2: %04u", ADC1->JDR3);
	LCD_Print(&blcd, 0, 16 * 4, "PA3: %04u", ADC1->JDR4);
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

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc == &hadc1)
	{
		SampleProcess();
	}
}
