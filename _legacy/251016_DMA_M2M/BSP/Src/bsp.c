#include "bsp.h"

#define DATA_SIZE			32

const uint32_t srcData[DATA_SIZE] = {
	0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F,
	0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
	0x20212223, 0x24252627, 0x28292A2B, 0x2C2D2E2F,
	0x30313233, 0x34353637, 0x38393A3B, 0x3C3D3E3F,
	0x40414243, 0x44454647, 0x48494A4B, 0x4C4D4E4F,
	0x50515253, 0x54555657, 0x58595A5B, 0x5C5D5E5F,
	0x60616263, 0x64656667, 0x68696A6B, 0x6C6D6E6F,
	0x70717273, 0x74757677, 0x78797A7B, 0x7C7D7E7F,
};

uint32_t dstData_cpu[DATA_SIZE];
uint32_t dstData_dma[DATA_SIZE];

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	/* Transfer data through CPU */
	memcpy(dstData_cpu, srcData, DATA_SIZE << 2);
	
	/* Transfer data through DMA */
	HAL_DMA_Start(&hdma_memtomem_dma1_channel1, (uint32_t)srcData, (uint32_t)dstData_dma, DATA_SIZE);
	
	while (1)
	{
		
	}
}
