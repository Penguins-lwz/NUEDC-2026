#ifndef __BSP_FFT_F32_H__
#define __BSP_FFT_F32_H__

#include "bsp.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define FFT_CALC_SIZE		512U

void BSP_FFT_f32(const float32_t *pSrc, float32_t *pDst);

#endif /* __BSP_FFT_F32_H__ */
