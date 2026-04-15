#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "bsp.h"

#define UART_BUF_SIZE		128U				// including NUL('\0')

typedef struct {
	UART_Regs *Instance;
	uint32_t dma_tx;
	__IO uint32_t gState;
	__IO uint32_t RxLen;
	char *pTxMsg;
	char *pRxMsg;
} BSP_UART_HandleTypeDef;

void BSP_UART_ReceiveToIdle_DMA(BSP_UART_HandleTypeDef *buart);
void BSP_UART_Transmit_DMA(BSP_UART_HandleTypeDef *buart, const char *format, ...);

void BSP_UART_IRQHandler(BSP_UART_HandleTypeDef *buart);
void BSP_UART_RxCpltCallback(BSP_UART_HandleTypeDef *buart, uint16_t Size);

#endif /* __BSP_UART_H__ */
