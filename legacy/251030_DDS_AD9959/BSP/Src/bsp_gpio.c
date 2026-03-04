#include "bsp_gpio.h"

uint8_t KEY_Read_Basic(void)
{
	uint8_t status = 0x00;
	if (KEY_S1_ACTIVE) status |= KEY_NUM_S1;
	if (KEY_S2_ACTIVE) status |= KEY_NUM_S2;
	if (KEY_S3_ACTIVE) status |= KEY_NUM_S3;
	if (KEY_S4_ACTIVE) status |= KEY_NUM_S4;
	
	static uint16_t cnt = 0;
	static uint8_t unionStatus = 0x00;
	if (status) { cnt++, unionStatus |= status; return 0x00; }
	if (cnt >= KEY_TH_HOLD) status = unionStatus | KEY_FLAG_HOLD; else
	if (cnt >= KEY_TH_TAP)  status = unionStatus | KEY_FLAG_TAP;
	cnt = 0, unionStatus = 0x00;
	return status;
}

uint8_t KEY_Read_Advanced(void)
{
	uint8_t status = 0x00;
	if (KEY_S1_ACTIVE) status |= KEY_NUM_S1;
	if (KEY_S2_ACTIVE) status |= KEY_NUM_S2;
	if (KEY_S3_ACTIVE) status |= KEY_NUM_S3;
	if (KEY_S4_ACTIVE) status |= KEY_NUM_S4;
	
	static uint16_t cnt = 0, idleCnt = 0;
	static uint8_t unionStatus = 0x00, lastStatus = 0x00;
	if (status) { cnt++, idleCnt = 0, unionStatus |= status; return 0x00; }
	if (lastStatus == 0x00)
	{
		if (cnt >= KEY_TH_HOLD) status = unionStatus | KEY_FLAG_HOLD; else
		if (cnt >= KEY_TH_TAP)  lastStatus = unionStatus;
	}
	else if (++idleCnt >= KEY_TH_WAIT)
		status = lastStatus | KEY_FLAG_TAP, lastStatus = 0x00;
	else if (cnt >= KEY_TH_TAP && unionStatus == lastStatus)
		status = lastStatus | KEY_FLAG_DOUBLE, lastStatus = 0x00;
	cnt = 0, unionStatus = 0x00;
	return status;
}
