#ifndef __BSP_H__
#define __BSP_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "dma.h"

/* Macro for STM32H7 devices */
#define __ZI_AXI_SRAM		__attribute__((section(".bss.RAM_D1")))
#define __RW_AXI_SRAM		__attribute__((section(".data.RAM_D1")))

/* Macro for GPIO control */
#define IO_RD(Name)			HAL_GPIO_ReadPin(Name##_GPIO_Port, Name##_Pin)
#define IO_WR(Name, Val)	HAL_GPIO_WritePin(Name##_GPIO_Port, Name##_Pin, Val)
#define IO_TG(Name)			HAL_GPIO_TogglePin(Name##_GPIO_Port, Name##_Pin)

void MainProcess(void);

#endif /* __BSP_H__ */
