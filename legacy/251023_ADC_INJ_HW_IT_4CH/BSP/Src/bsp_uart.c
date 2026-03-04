#include "bsp_uart.h"

void BSP_UART_ReceiveToIdle_DMA(BSP_UART_HandleTypeDef *buart)
{
	memset(buart->pRxMsg, 0, UART_BUF_SIZE);
	HAL_UARTEx_ReceiveToIdle_DMA(buart->huart, (uint8_t *)buart->pRxMsg, UART_BUF_SIZE - 1);
	__HAL_DMA_DISABLE_IT(buart->huart->hdmarx, DMA_IT_HT);
}

void BSP_UART_Transmit_DMA(BSP_UART_HandleTypeDef *buart, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	while (buart->huart->gState != HAL_UART_STATE_READY);
	vsnprintf(buart->pTxMsg, UART_BUF_SIZE, format, ap);
	va_end(ap);
	HAL_UART_Transmit_DMA(buart->huart, (uint8_t *)buart->pTxMsg, strlen(buart->pTxMsg));
}
