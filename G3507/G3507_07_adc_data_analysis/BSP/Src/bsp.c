#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define PI					3.14159265358979f
#define VREF				3300.0f

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

char     guart0_TxMsg[UART_BUF_SIZE];
char     guart0_RxMsg[UART_BUF_SIZE];
uint16_t gadc_ConvData[ADC_BUF_SIZE] __attribute__((aligned(4)));
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

__IO uint32_t adcConvCpltFlag = 0;

APP_SIG_HandleTypeDef sig_ch1;
APP_SIG_HandleTypeDef sig_ch2;

BSP_UART_HandleTypeDef buart = {
	.Instance = UART_0_INST,
	.dma_tx   = DMA_CH0_CHAN_ID,
	.pTxMsg   = guart0_TxMsg,
	.pRxMsg   = guart0_RxMsg,
};

LCD_HandleTypeDef blcd = {
	.Instance = &LCD_1_80_inch,
	.dir      = LCD_DIR_RIGHT,
	.TxBuf    = glcd_TxBuf,
};

static void PeriodicProcess(void);
static void SampleProcess(void);

int main(void)
{
	SYSCFG_DL_init();
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMG_0_INST_INT_IRQN);
	NVIC_EnableIRQ(ADC_0_INST_INT_IRQN);
	
	LCD_Init(&blcd, &LCD_Font_1608, NULL, WHITE, BLACK);
	LCD_Print(&blcd, 0, 16 * 0, "       PA27    PA26 ");
	LCD_Print(&blcd, 0, 16 * 1, " MIN  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 2, " MAX  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 3, " AMP  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 4, " AVG  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 5, " RMS  0000.0  0000.0");
	LCD_Print(&blcd, 0, 16 * 6, " PHS   000.0   000.0");
	LCD_Print(&blcd, 0, 16 * 7, "DIFF   000.0        ");
	
	DL_Timer_startCounter(TIMG_0_INST);
	
	while (1)
	{
		if (adcConvCpltFlag)
		{
			adcConvCpltFlag = 0;
			for (uint32_t i = 0; i < ADC_PER_CHAN; ++i)
				BSP_UART_Transmit_DMA(&buart, "[%03u]: %u, %u\n", i, gadc_ConvData[i * 2], gadc_ConvData[i * 2 + 1]);
		}
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus)
	{
		DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, DL_ADC12_getFIFOAddress(ADC_0_INST));
		DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)gadc_ConvData);
		DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, ADC_BUF_SIZE >> 1);
		DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
		
		DL_Timer_startCounter(TIMG_8_INST);
	}
	
	static uint32_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
}

static void SignalAnalysis(APP_SIG_HandleTypeDef *asig, const uint16_t *pData, uint32_t sample_per_period)
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
	DL_Timer_stopCounter(TIMG_8_INST);
	DL_DMA_disableChannel(DMA, DMA_CH1_CHAN_ID);
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

void BSP_UART_RxCpltCallback(BSP_UART_HandleTypeDef *buart, uint16_t Size)
{
	BSP_UART_Transmit_DMA(buart, "RxLen: %u, RxMsg: %s", Size, buart->pRxMsg);
}

void UART_0_INST_IRQHandler(void)
{
	BSP_UART_IRQHandler(&buart);
}

void TIMG_0_INST_IRQHandler(void)
{
	switch (DL_Timer_getPendingInterrupt(TIMG_0_INST))
	{
		case DL_TIMER_IIDX_ZERO:
			PeriodicProcess();
			break;
		default:
			break;
	}
}

void ADC_0_INST_IRQHandler(void)
{
	switch (DL_ADC12_getPendingInterrupt(ADC_0_INST))
	{
		case DL_ADC12_IIDX_DMA_DONE:
			SampleProcess();
			break;
		default:
			break;
	}
}
