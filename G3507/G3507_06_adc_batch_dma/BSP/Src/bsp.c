#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

#define ADC_BUF_SIZE		100U

char     guart0_TxMsg[UART_BUF_SIZE];
char     guart0_RxMsg[UART_BUF_SIZE];
uint16_t gadc_ConvData[ADC_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

__IO uint32_t adcConvCpltFlag = 0;

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
	LCD_Print(&blcd, 0, 16 * 0, "ADC Batch DMA");
	LCD_Print(&blcd, 0, 16 * 1, "PA27 -> UART");
	
	DL_Timer_startCounter(TIMG_0_INST);
	
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
		DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, DL_ADC12_getMemResultAddress(ADC_0_INST, ADC_0_ADCMEM_0));
		DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)gadc_ConvData);
		DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, ADC_BUF_SIZE);
		DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
		
		DL_Timer_startCounter(TIMG_8_INST);
	}
	
	static uint32_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
}

static void SampleProcess(void)
{
	DL_Timer_stopCounter(TIMG_8_INST);
	DL_DMA_disableChannel(DMA, DMA_CH1_CHAN_ID);
	adcConvCpltFlag = 1;
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
