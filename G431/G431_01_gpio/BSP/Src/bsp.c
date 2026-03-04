#include "bsp.h"
#include "bsp_gpio.h"

static void PeriodicProcess(void);

void MainProcess(void)
{
	LED_WR(0x00);
	
	HAL_TIM_Base_Start_IT(&htim15);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	if (keyStatus) LED_WR(keyStatus);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim15)
	{
		PeriodicProcess();
	}
}
