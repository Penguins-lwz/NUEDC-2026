#include "bsp.h"
#include "bsp_gpio.h"

static void PeriodicProcess(void);

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	HAL_TIM_Base_Start_IT(&htim17);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	switch (keyStatus)
	{
		case KEY_S1_TAP : LED_ON(R);                       break;
		case KEY_S1_HOLD: LED_OF(R);                       break;
		case KEY_S2_TAP :            LED_ON(G);            break;
		case KEY_S2_HOLD:            LED_OF(G);            break;
		case KEY_S3_TAP :                       LED_ON(B); break;
		case KEY_S3_HOLD:                       LED_OF(B); break;
		case KEY_S4_TAP : LED_ON(R); LED_ON(G); LED_ON(B); break;
		case KEY_S4_HOLD: LED_OF(R); LED_OF(G); LED_OF(B); break;
		default: break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim17)
	{
		PeriodicProcess();
	}
}
