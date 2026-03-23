#include "bsp.h"
#include "bsp_gpio.h"

static void PeriodicProcess(void);

int main(void)
{
	SYSCFG_DL_init();
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_0_INST);
	
	while (1)
	{
		
	}
}

static void PeriodicProcess(void)
{
	uint8_t keyStatus = KEY_Read();
	
	switch (keyStatus)
	{
		case KEY_S1_TAP    : LED_ON(R); break;
		case KEY_S1_HOLD   : LED_OF(R); break;
		case KEY_S2_TAP    : LED_ON(G); break;
		case KEY_S2_HOLD   : LED_OF(G); break;
		case KEY_S1_S2_TAP : LED_ON(B); break;
		case KEY_S1_S2_HOLD: LED_OF(B); break;
		default: break;
	}
}

void TIMER_0_INST_IRQHandler(void)
{
	switch (DL_Timer_getPendingInterrupt(TIMER_0_INST))
	{
		case DL_TIMER_IIDX_ZERO:
			PeriodicProcess();
			break;
		default:
			break;
	}
}
