#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "bsp.h"
#include "bsp_font.h"

#define LCD_BUF_SIZE		512U

typedef enum {
	LCD_DIR_TOP    = 0x01U,
	LCD_DIR_LEFT   = 0x02U,
	LCD_DIR_RIGHT  = 0x03U,
	LCD_DIR_BOTTOM = 0x00U,
} LCD_DirTypeDef;

// CSS Color Module Level 3 in RGB565, big-endian
typedef enum {
	BLACK   = 0x0000U,		// #000000
	RED     = 0x00F8U,		// #FF0000
	ORANGE  = 0x20FDU,		// #FFA500
	YELLOW  = 0xE0FFU,		// #FFFF00
	LIME    = 0xE007U,		// #00FF00
	AQUA    = 0xFF07U,		// #00FFFF
	BLUE    = 0x1F00U,		// #0000FF
	FUCHSIA = 0x1FF8U,		// #FF00FF
	WHITE   = 0xFFFFU,		// #FFFFFF
	SILVER  = 0x18C6U,		// #C0C0C0
	GRAY    = 0x1084U,		// #808080
} LCD_ColorTypeDef;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t inverse;
	uint16_t xBias[4];
	uint16_t yBias[4];
} LCD_TypeDef;

typedef struct {
	const LCD_TypeDef     *Instance;
	const LCD_FontTypeDef *font_en;
	const LCD_FontTypeDef *font_cn;
	SPI_HandleTypeDef     *hspi;
	LCD_ColorTypeDef      foreColor;
	LCD_ColorTypeDef      backColor;
	LCD_DirTypeDef        dir;
	uint16_t              xLen;
	uint16_t              yLen;
	uint16_t              *TxBuf;
} LCD_HandleTypeDef;

extern const LCD_TypeDef LCD_1_69_inch;
extern const LCD_TypeDef LCD_1_80_inch;
extern const LCD_TypeDef LCD_2_00_inch;

void LCD_Init(LCD_HandleTypeDef *blcd, const LCD_FontTypeDef *font_en, const LCD_FontTypeDef *font_cn, LCD_ColorTypeDef fc, LCD_ColorTypeDef bc);
void LCD_ConfigFont(LCD_HandleTypeDef *blcd, const LCD_FontTypeDef *font_en, const LCD_FontTypeDef *font_cn, LCD_ColorTypeDef fc, LCD_ColorTypeDef bc);

void LCD_Clear(LCD_HandleTypeDef *blcd, LCD_ColorTypeDef color);
void LCD_Fill(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, uint16_t xsize, uint16_t ysize, LCD_ColorTypeDef color);

void LCD_Print(LCD_HandleTypeDef *blcd, uint16_t xpos, uint16_t ypos, const char *format, ...);

#endif
