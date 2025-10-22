#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define ADC_BUF_SIZE		100U

char     guart1_TxMsg[UART_BUF_SIZE];
char     guart1_RxMsg[UART_BUF_SIZE];
uint16_t gadc_ConvData[ADC_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

__IO uint32_t adcConvCpltFlag = 0;

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
	LCD_Print(&blcd, 0, 16 * 0, "REG_HW_DMA_1CH");
	
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim17);
	
	while (1)
	{
		if (adcConvCpltFlag)
		{
			adcConvCpltFlag = 0;
			for (uint32_t i = 0; i < ADC_BUF_SIZE; ++i)
				BSP_UART_Transmit_DMA(&buart, "[%02u]: %u\n", i, gadc_ConvData[i]);
		}
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus)
	{
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)gadc_ConvData, ADC_BUF_SIZE);
		HAL_TIM_Base_Start(&htim6);
	}
	
	static uint16_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
}

static void SampleProcess(void)
{
	HAL_TIM_Base_Stop(&htim6);
	HAL_ADC_Stop_DMA(&hadc1);
	adcConvCpltFlag = 1;
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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc == &hadc1)
	{
		SampleProcess();
	}
}
