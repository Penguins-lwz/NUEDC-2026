#include "bsp_ad9959.h"

static void DDS_Transmit(uint8_t data)
{
	for (uint8_t idx = 0x80; idx; idx >>= 1)
	{
		IO_WR(DDS_SDA, data & idx);
		IO_WR(DDS_SCL, 1);
		IO_WR(DDS_SCL, 0);
	}
}

void DEV_DDS_Init(void)
{
	IO_WR(DDS_RST, 1);
	IO_WR(DDS_RST, 0);
	IO_WR(DDS_CS, 0);
	
	DDS_Transmit(0x01);
	DDS_Transmit(0xD0);
	DDS_Transmit(0x00);
	DDS_Transmit(0x00);
	
	DDS_Transmit(0x02);
	DDS_Transmit(0x20);
	DDS_Transmit(0x00);
}

void DEV_DDS_Config(uint32_t chan, float freq, float phs, uint32_t amp)
{
	uint32_t tmp;
	
	DDS_Transmit(0x00);
	DDS_Transmit(chan);
	
	tmp = freq * (1ULL << 32) / DDS_MCLK + .5f;
	DDS_Transmit(0x04);
	DDS_Transmit(tmp >> 24);
	DDS_Transmit(tmp >> 16);
	DDS_Transmit(tmp >> 8);
	DDS_Transmit(tmp >> 0);
	
	tmp = phs * (1ULL << 14) / 360 + .5f;
	DDS_Transmit(0x05);
	DDS_Transmit(tmp >> 8);
	DDS_Transmit(tmp >> 0);
	
	tmp = amp & 0x03FFU | 0x1000U;
	DDS_Transmit(0x06);
	DDS_Transmit(0x00);
	DDS_Transmit(tmp >> 8);
	DDS_Transmit(tmp >> 0);
	
	IO_WR(DDS_UPD, 1);
	IO_WR(DDS_UPD, 0);
}
