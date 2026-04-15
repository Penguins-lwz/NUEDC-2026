#ifndef __BSP_H__
#define __BSP_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ti_msp_dl_config.h"

/* Macro for GPIO control */
#define IO_RD(Name)			DL_GPIO_readPins(Name##_PORT, Name##_PIN)
#define IO_WR(Name, Val)	DL_GPIO_writePinsVal(Name##_PORT, Name##_PIN, (Val) ? Name##_PIN : 0)
#define IO_TG(Name)			DL_GPIO_togglePins(Name##_PORT, Name##_PIN)

#endif /* __BSP_H__ */
