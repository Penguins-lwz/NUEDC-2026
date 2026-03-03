#ifndef __BSP_AT24C02_H__
#define __BSP_AT24C02_H__

#include "bsp.h"
#include "i2c_hal.h"

void ROM_Init(void);
void ROM_Write(uint8_t addr, uint8_t data);
uint8_t ROM_Read(uint8_t addr);

#endif /* __BSP_AT24C02_H__ */
