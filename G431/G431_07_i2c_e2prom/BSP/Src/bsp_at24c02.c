#include "bsp_at24c02.h"

void ROM_Init(void)
{
	I2CInit();
}

void ROM_Write(uint8_t addr, uint8_t data)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(data);
	I2CWaitAck();
	
	I2CStop();
}

uint8_t ROM_Read(uint8_t addr)
{
	uint8_t data;
	
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA1);
	I2CWaitAck();
	data = I2CReceiveByte();
	I2CSendNotAck();
	
	I2CStop();
	return data;
}
