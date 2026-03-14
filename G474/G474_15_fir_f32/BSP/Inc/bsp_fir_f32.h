#ifndef __BSP_FIR_F32_H__
#define __BSP_FIR_F32_H__

#include "bsp.h"

#define FIR_TAP_SIZE		49U
#define FIR_SRC_SIZE		288U
#define FIR_DST_SIZE		(FIR_SRC_SIZE - FIR_TAP_SIZE + 1U)
#define FIR_PAIRS			(FIR_TAP_SIZE >> 1U)

typedef float float32_t;

void BSP_FIR_f32(const float32_t *pSrc, float32_t *pDst);

#endif /* __BSP_FIR_F32_H__ */
