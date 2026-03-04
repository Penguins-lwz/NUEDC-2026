#ifndef __BSP_AD9959_H__
#define __BSP_AD9959_H__

#include "bsp.h"

#define DDS_MCLK			500000000U

#define DDS_CHANNEL_0		0x10U
#define DDS_CHANNEL_1		0x20U
#define DDS_CHANNEL_2		0x40U
#define DDS_CHANNEL_3		0x80U

void DEV_DDS_Init(void);
void DEV_DDS_Config(uint32_t chan, float freq, float phs, uint32_t amp);

#endif /* __BSP_AD9959_H__ */
