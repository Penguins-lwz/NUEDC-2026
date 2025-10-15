#include "bsp.h"
#include "bsp_gpio.h"

enum {
	LOCK,
	UNLOCK_R,
	UNLOCK_G,
	UNLOCK_B,
} status = LOCK;

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
	
	if (status == LOCK) status = keyStatus == KEY_S1_HOLD ? UNLOCK_R : LOCK;
	else if (keyStatus == KEY_S2_TAP || keyStatus == KEY_S2_HOLD) status = UNLOCK_G;
	else if (keyStatus == KEY_S3_TAP || keyStatus == KEY_S3_HOLD) status = UNLOCK_B;
	else if (keyStatus == (KEY_S1_HOLD | KEY_S4_HOLD)) status = LOCK;
	
	static uint32_t cnt = 0;
	cnt = (cnt + 1) % 200;
	IO_WR(LED_R, status != UNLOCK_R || cnt / 100 % 2);
	IO_WR(LED_G, status != UNLOCK_G || cnt / 50  % 2);
	IO_WR(LED_B, status != UNLOCK_B || cnt / 50  % 2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim17)
	{
		PeriodicProcess();
	}
}
