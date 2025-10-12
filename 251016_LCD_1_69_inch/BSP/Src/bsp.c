#include "bsp.h"
#include "bsp_lcd.h"

uint16_t glcd_TxBuf[LCD_BUF_SIZE];

LCD_HandleTypeDef blcd = {
	.Instance = &LCD_1_69_inch,
	.hspi     = &hspi4,
	.dir      = LCD_DIR_LEFT,
	.TxBuf    = glcd_TxBuf,
};

void MainProcess(void)
{
	/* Note: This project is overclocked to 200 MHz automatically. */
	
	LCD_Init(&blcd, &LCD_Font_1608, &LCD_Font_1616, WHITE, BLACK);
	
	LCD_Print(&blcd, 52, 12 *  2, " 　　华中科技大学　　 ");
	LCD_Print(&blcd, 52, 12 *  4, " 电工电子科技创新中心 ");
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, ORANGE,  BLACK);
	LCD_Print(&blcd, 52, 12 *  7, "STM32G474VET6 LCD Demo");
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, AQUA,    BLACK);
	LCD_Print(&blcd, 52, 12 * 10, "   Unique device ID   ");
	LCD_Print(&blcd, 36, 12 * 12, "%08X %08X %08X", HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0());
	
	LCD_ConfigFont(&blcd, &LCD_Font_1608, NULL, FUCHSIA, BLACK);
	LCD_Print(&blcd, 52, 12 * 15, "     Penguins-lwz     ");
	
	while (1)
	{
		
	}
}
