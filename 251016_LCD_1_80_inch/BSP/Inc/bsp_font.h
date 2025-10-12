#ifndef __BSP_FONT_H__
#define __BSP_FONT_H__ 	   

#include "bsp.h"

typedef struct {
	uint16_t xSize;
	uint16_t ySize;
	uint16_t nByte;
	uint8_t *addr;
	uint8_t *map;
} LCD_FontTypeDef;

extern const LCD_FontTypeDef LCD_Font_1206;
extern const LCD_FontTypeDef LCD_Font_1608;
extern const LCD_FontTypeDef LCD_Font_2412;
extern const LCD_FontTypeDef LCD_Font_3216;
extern const LCD_FontTypeDef LCD_Font_1616;

#endif
