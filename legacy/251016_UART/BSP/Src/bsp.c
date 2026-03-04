#include "bsp.h"
#include "bsp_uart.h"

char guart1_TxMsg[UART_BUF_SIZE];
char guart1_RxMsg[UART_BUF_SIZE];

BSP_UART_HandleTypeDef buart = {
	.huart  = &huart1,
	.pTxMsg = guart1_TxMsg,
	.pRxMsg = guart1_RxMsg,
};

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	BSP_UART_Transmit_DMA(&buart, "UIDw2: 0x%08X\n", HAL_GetUIDw2());
	BSP_UART_Transmit_DMA(&buart, "UIDw1: 0x%08X\n", HAL_GetUIDw1());
	BSP_UART_Transmit_DMA(&buart, "UIDw0: 0x%08X\n", HAL_GetUIDw0());
	
	BSP_UART_ReceiveToIdle_DMA(&buart);
	
	while (1)
	{
		
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart == buart.huart)
	{
		BSP_UART_Transmit_DMA(&buart, "RxLen: %u, RxMsg: %s", Size, buart.pRxMsg);
		BSP_UART_ReceiveToIdle_DMA(&buart);
	}
}
