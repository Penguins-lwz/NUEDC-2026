#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define PI					3.14159265358979f
#define VREF				3000.0f

#define ADC_PER_CHAN		800U
#define ADC_BUF_SIZE		1600U

typedef struct {
	uint16_t min;
	uint16_t max;
	uint16_t amp;
	float    avg;
	float    rms;
	float    phs;
} APP_SIG_HandleTypeDef;

char     guart1_TxMsg[UART_BUF_SIZE];
char     guart1_RxMsg[UART_BUF_SIZE];
uint16_t gadc_ConvData[ADC_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

__IO uint32_t adcConvCpltFlag = 0;

APP_SIG_HandleTypeDef sig_ch1;
APP_SIG_HandleTypeDef sig_ch2;

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
	
	LCD_Init(&blcd, &LCD_Font_1608, NULL, WHITE, BLACK);
	LCD_Print(&blcd, 0, 16 * 0, "       PA0     PA1  ");
	LCD_Print(&blcd, 0, 16 * 1, " MIN  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 2, " MAX  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 3, " AMP  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 4, " AVG  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 5, " RMS  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 6, " PHS   000.0   000.0");
	LCD_Print(&blcd, 0, 16 * 7, "DIFF   000.0        ");
	
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	HAL_TIM_Base_Start_IT(&htim17);
	
	while (1)
	{
		if (adcConvCpltFlag)
		{
			adcConvCpltFlag = 0;
			for (uint32_t i = 0; i < ADC_PER_CHAN; ++i)
				BSP_UART_Transmit_DMA(&buart, "[%02u]: %u, %u\n", i, gadc_ConvData[i * 2], gadc_ConvData[i * 2 + 1]);
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

static void SignalAnalysis(APP_SIG_HandleTypeDef *asig, uint16_t *pData, uint32_t sample_per_period)
{
	uint16_t val;
	uint16_t min  = 4095;
	uint16_t max  = 0;
	float    sum1 = 0;
	float    sum2 = 0;
	float    real = 0;
	float    imag = 0;
	float    phs  = 0;
	float    ang;
	
	for (uint32_t i = 0; i < sample_per_period; ++i, pData += 2)
	{
		val   = *pData;
		ang   = 2.0f * PI * i / sample_per_period;
		min   = val < min ? val : min;
		max   = val > max ? val : max;
		sum1 += val;
		sum2 += (uint32_t)val * val;
		real += val * cosf(ang);
		imag += val * sinf(ang);
	}
	
	asig->min = min;
	asig->max = max;
	asig->amp = max - min;
	asig->avg = sum1 / sample_per_period;
	asig->rms = sqrtf(sum2 / sample_per_period - asig->avg * asig->avg);
	asig->phs = atan2f(-imag, real);
}

static void SampleProcess(void)
{
	HAL_TIM_Base_Stop(&htim6);
	HAL_ADC_Stop_DMA(&hadc1);
	adcConvCpltFlag = 1;
	
	SignalAnalysis(&sig_ch1, gadc_ConvData + ADC_PER_CHAN / 4    , ADC_PER_CHAN / 2);
	SignalAnalysis(&sig_ch2, gadc_ConvData + ADC_PER_CHAN / 4 + 1, ADC_PER_CHAN / 2);
	
	LCD_Print(&blcd, 8 * 6, 16 * 1, "%6.1f  %6.1f", sig_ch1.min * VREF / 4096, sig_ch2.min * VREF / 4096);
	LCD_Print(&blcd, 8 * 6, 16 * 2, "%6.1f  %6.1f", sig_ch1.max * VREF / 4096, sig_ch2.max * VREF / 4096);
	LCD_Print(&blcd, 8 * 6, 16 * 3, "%6.1f  %6.1f", sig_ch1.amp * VREF / 4096, sig_ch2.amp * VREF / 4096);
	LCD_Print(&blcd, 8 * 6, 16 * 4, "%6.1f  %6.1f", sig_ch1.avg * VREF / 4096, sig_ch2.avg * VREF / 4096);
	LCD_Print(&blcd, 8 * 6, 16 * 5, "%6.1f  %6.1f", sig_ch1.rms * VREF / 4096, sig_ch2.rms * VREF / 4096);
	LCD_Print(&blcd, 8 * 6, 16 * 6, "%6.1f  %6.1f", sig_ch1.phs * 180.0f / PI, sig_ch2.phs * 180.0f / PI);
	LCD_Print(&blcd, 8 * 6, 16 * 7, "%6.1f",                   (sig_ch2.phs - sig_ch1.phs) * 180.0f / PI);
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
