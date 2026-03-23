#include "bsp.h"
#include "bsp_gpio.h"
#include "bsp_lcd.h"

uint16_t glcd_TxBuf[LCD_BUF_SIZE];

LCD_HandleTypeDef blcd = {
	.Instance = &LCD_1_80_inch,
	.dir      = LCD_DIR_RIGHT,
	.TxBuf    = glcd_TxBuf,
};

static void PeriodicProcess(void);

int main(void)
{
	SYSCFG_DL_init();
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	
	LCD_Init(&blcd, &LCD_Font_1608, &LCD_Font_1616, WHITE, BLACK);
	
	LCD_Print(&blcd, 0, 16 * 0, "　　华中科技大学　　");
	LCD_Print(&blcd, 0, 16 * 1, "电工电子科技创新中心");
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, ORANGE,  BLACK);
	LCD_Print(&blcd, 0, 16 * 2, " LP-MSPM0G3507 Demo ");
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, AQUA,    BLACK);
	LCD_Print(&blcd, 0, 16 * 3, "  Unique device ID  ");
	LCD_Print(&blcd, 0, 16 * 4, "      %08X      ", FACTORYREGION->DEVICEID);
	LCD_Print(&blcd, 0, 16 * 5, "      %08X      ", FACTORYREGION->USERID);
	LCD_Print(&blcd, 0, 16 * 6, "      %08X      ", FACTORYREGION->TRACEID);
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, FUCHSIA, BLACK);
	LCD_Print(&blcd, 0, 16 * 7, "    Penguins-lwz    ");
	
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
