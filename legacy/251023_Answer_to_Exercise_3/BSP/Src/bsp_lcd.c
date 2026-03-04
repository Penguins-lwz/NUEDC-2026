#include "bsp_lcd.h"

#define LCD_ConfigReg(__blcd__, ...) \
        LCD_WriteRegData(__blcd__, sizeof((uint8_t[]){__VA_ARGS__}) / sizeof(uint8_t), __VA_ARGS__)

const uint8_t lcd_dir_code[4] = { 0x00, 0xC0, 0x60, 0xA0 };

const LCD_TypeDef LCD_1_69_inch = {
	.width   = 240,
	.height  = 280,
	.inverse = 0x21,
	.xBias   = {  0,  0, 20, 20 },
	.yBias   = { 20, 20,  0,  0 },
};

const LCD_TypeDef LCD_1_80_inch = {
	.width   = 128,
	.height  = 160,
	.inverse = 0x20,
	.xBias   = { 2, 2, 1, 1 },
	.yBias   = { 1, 1, 2, 2 },
};

const LCD_TypeDef LCD_2_00_inch = {
	.width   = 240,
	.height  = 320,
	.inverse = 0x21,
	.xBias   = { 0, 0, 0, 0 },
	.yBias   = { 0, 0, 0, 0 },
};

static void LCD_Transmit(LCD_HandleTypeDef *blcd, uint8_t *pData, uint16_t size)
{
	HAL_SPI_Transmit(blcd->hspi, pData, size, 0xFFFF);
}

static void LCD_WriteRegData(LCD_HandleTypeDef *blcd, uint32_t cnt, ...)
{
	va_list ap;
	va_start(ap, cnt);
	
	uint8_t *ptr = (uint8_t *)blcd->TxBuf;
	for (uint8_t i = 0; i < cnt; ++i)
		ptr[i] = va_arg(ap, uint32_t);
	
	IO_WR(LCD_DC, 0);
	LCD_Transmit(blcd, ptr, 1);
	
	IO_WR(LCD_DC, 1);
	if (--cnt) LCD_Transmit(blcd, ++ptr, cnt);
	
	va_end(ap);
}

void LCD_Init(LCD_HandleTypeDef *blcd, const LCD_FontTypeDef *font_en, const LCD_FontTypeDef *font_cn, LCD_ColorTypeDef fc, LCD_ColorTypeDef bc)
{
	switch (blcd->dir)
	{
		case LCD_DIR_TOP:
		case LCD_DIR_BOTTOM:
			blcd->xLen = blcd->Instance->width;
			blcd->yLen = blcd->Instance->height;
			break;
		
		case LCD_DIR_LEFT:
		case LCD_DIR_RIGHT:
			blcd->xLen = blcd->Instance->height;
			blcd->yLen = blcd->Instance->width;
			break;
	}
	
	IO_WR(LCD_RST, 0);
	IO_WR(LCD_RST, 1);
	HAL_Delay(5);
	
	IO_WR(LCD_CS, 0);
	LCD_ConfigReg(blcd, 0x11);
	HAL_Delay(5);
	
	LCD_ConfigReg(blcd, 0x36, lcd_dir_code[blcd->dir]);
	LCD_ConfigReg(blcd, 0x3A, 0x55);
	LCD_ConfigReg(blcd, blcd->Instance->inverse);
	LCD_ConfigReg(blcd, 0x29);
	
	LCD_ConfigFont(blcd, font_en, font_cn, fc, bc);
	LCD_Clear(blcd, bc);
}

void LCD_ConfigFont(LCD_HandleTypeDef *blcd, const LCD_FontTypeDef *font_en, const LCD_FontTypeDef *font_cn, LCD_ColorTypeDef fc, LCD_ColorTypeDef bc)
{
	blcd->font_en   = font_en;
	blcd->font_cn   = font_cn;
	blcd->foreColor = fc;
	blcd->backColor = bc;
}

static void LCD_SetAddress(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, uint16_t xsize, uint16_t ysize)
{
	uint16_t x1 = xpos + blcd->Instance->xBias[blcd->dir], x2 = x1 + xsize - 1;
	uint16_t y1 = ypos + blcd->Instance->yBias[blcd->dir], y2 = y1 + ysize - 1;
	LCD_ConfigReg(blcd, 0x2A, x1 >> 8, x1, x2 >> 8, x2);
	LCD_ConfigReg(blcd, 0x2B, y1 >> 8, y1, y2 >> 8, y2);
	LCD_ConfigReg(blcd, 0x2C);
}

void LCD_Clear(LCD_HandleTypeDef *blcd, LCD_ColorTypeDef color)
{
	LCD_Fill(blcd, 0, 0, blcd->xLen, blcd->yLen, color);
}

void LCD_Fill(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, uint16_t xsize, uint16_t ysize, LCD_ColorTypeDef color)
{
	LCD_SetAddress(blcd, xpos, ypos, xsize, ysize);
	for (uint16_t i = 0; i < xsize; ++i)
		blcd->TxBuf[i] = color;
	for (uint16_t i = 0; i < ysize; ++i)
		LCD_Transmit(blcd, (uint8_t *)blcd->TxBuf, xsize * 2);
}

static void LCD_PrintChar_en(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, char *ptr)
{
	if (xpos + blcd->font_en->xSize > blcd->xLen) return;
	if (ypos + blcd->font_en->ySize > blcd->yLen) return;
	
	uint8_t *addr = blcd->font_en->addr + blcd->font_en->nByte * (*ptr - 32);
	LCD_SetAddress(blcd, xpos, ypos, blcd->font_en->xSize, blcd->font_en->ySize);
	for (uint16_t i = 0; i < blcd->font_en->nByte * 8; ++i)
		blcd->TxBuf[i] = addr[i / 8] & (0x01 << (i % 8)) ? blcd->foreColor : blcd->backColor;
	LCD_Transmit(blcd, (uint8_t *)blcd->TxBuf, blcd->font_en->nByte * 16);
}

static void LCD_PrintChar_cn(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, char *ptr)
{
	if (xpos + blcd->font_cn->xSize > blcd->xLen) return;
	if (ypos + blcd->font_cn->ySize > blcd->yLen) return;
	
	uint16_t idx = 0;
	uint8_t *map = blcd->font_cn->map;
	while (map[idx] && (map[idx] != ptr[0] || map[idx + 1] != ptr[1])) idx += 2;
	if (map[idx] == NULL) return;
	
	uint8_t *addr = blcd->font_cn->addr + blcd->font_cn->nByte * idx / 2;
	LCD_SetAddress(blcd, xpos, ypos, blcd->font_cn->xSize, blcd->font_cn->ySize);
	for (uint16_t i = 0; i < blcd->font_cn->nByte * 8; ++i)
		blcd->TxBuf[i] = addr[i / 8] & (0x01 << (i % 8)) ? blcd->foreColor : blcd->backColor;
	LCD_Transmit(blcd, (uint8_t *)blcd->TxBuf, blcd->font_cn->nByte * 16);
}

void LCD_Print(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, const char *format, ...)
{
	char str[64];
	va_list ap;
	va_start(ap, format);
	vsnprintf(str, sizeof(str), format, ap);
	va_end(ap);
	
	char *ptr = str;
	while (*ptr)
	{
		if (*ptr >= 0x20 && *ptr < 0x7F)
		{
			LCD_PrintChar_en(blcd, xpos, ypos, ptr);
			xpos += blcd->font_en->xSize;
			ptr += 1;
		}
		else if (blcd->font_cn && *ptr >= 0x80)
		{
			LCD_PrintChar_cn(blcd, xpos, ypos, ptr);
			xpos += blcd->font_cn->xSize;
			ptr += 2;
		}
	}
}
