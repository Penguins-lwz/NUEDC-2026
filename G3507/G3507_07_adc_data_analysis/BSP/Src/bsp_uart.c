#include "bsp_uart.h"

void BSP_UART_Transmit_DMA(BSP_UART_HandleTypeDef *buart, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	while (buart->gState != 0U);
	vsnprintf(buart->pTxMsg, UART_BUF_SIZE, format, ap);
	va_end(ap);
	
	buart->gState = 1U;
	DL_DMA_setSrcAddr(DMA, buart->dma_tx, (uint32_t)buart->pTxMsg);
	DL_DMA_setDestAddr(DMA, buart->dma_tx, (uint32_t)(&buart->Instance->TXDATA));
	DL_DMA_setTransferSize(DMA, buart->dma_tx, strlen(buart->pTxMsg));
	DL_DMA_enableChannel(DMA, buart->dma_tx);
}

void BSP_UART_IRQHandler(BSP_UART_HandleTypeDef *buart)
{
	char uhdata;
	switch (DL_UART_getPendingInterrupt(UART_0_INST))
	{
		case DL_UART_IIDX_EOT_DONE:
			DL_DMA_disableChannel(DMA, buart->dma_tx);
			buart->gState = 0U;
			break;
		case DL_UART_IIDX_RX:
			uhdata = DL_UART_receiveData(buart->Instance);
			buart->pRxMsg[buart->RxLen++] = uhdata;
			if (uhdata == '\n' || buart->RxLen == UART_BUF_SIZE)
			{
				buart->pRxMsg[buart->RxLen] = 0;
				BSP_UART_RxCpltCallback(buart, buart->RxLen);
				buart->RxLen = 0;
			}
			break;
		default:
			break;
	}
}

SYSCONFIG_WEAK void BSP_UART_RxCpltCallback(BSP_UART_HandleTypeDef *buart, uint16_t Size)
{
	
}
