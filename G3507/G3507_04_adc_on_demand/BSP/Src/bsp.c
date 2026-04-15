#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_lcd.h"

char     guart0_TxMsg[UART_BUF_SIZE];
char     guart0_RxMsg[UART_BUF_SIZE];
uint16_t glcd_TxBuf[LCD_BUF_SIZE];

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

int main(void)
{
	SYSCFG_DL_init();
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMG_0_INST_INT_IRQN);
	
	LCD_Init(&blcd, &LCD_Font_1608, NULL, WHITE, BLACK);
	LCD_Print(&blcd, 0, 16 * 0, "ADC On-demand");
	
	DL_ADC12_startConversion(ADC_0_INST);
	
	DL_Timer_startCounter(TIMG_0_INST);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	static uint32_t timeCnt = 0;
	if (++timeCnt < 10) return;
	timeCnt = 0;
	
	LED_TG(R);
	BSP_UART_Transmit_DMA(&buart, "PA27: %u\n", DL_ADC12_getMemResult(ADC_0_INST, ADC_0_ADCMEM_0));
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
