#include "bsp_fft_f32.h"

#if FFT_CALC_SIZE == 16U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len16
#elif FFT_CALC_SIZE == 32U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len32
#elif FFT_CALC_SIZE == 64U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len64
#elif FFT_CALC_SIZE == 128U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len128
#elif FFT_CALC_SIZE == 256U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len256
#elif FFT_CALC_SIZE == 512U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len512
#elif FFT_CALC_SIZE == 1024U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len1024
#elif FFT_CALC_SIZE == 2048U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len2048
#elif FFT_CALC_SIZE == 4096U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len4096
#endif

const float32_t invMag1 = 1.0f / FFT_CALC_SIZE;
const float32_t invMag2 = 2.0f / FFT_CALC_SIZE;
const float32_t invPha  = 1.0f / PI;

void BSP_FFT_f32(const float32_t *pSrc, float32_t *pDst)
{
	float32_t *pTmp = pDst;
	for (uint32_t i = 0; i < FFT_CALC_SIZE; ++i)
		*pTmp++ = *pSrc++, *pTmp++ = 0.0f;
	
	arm_cfft_f32(&arm_cfft_sR_f32, pDst, 0, 1);
	pDst[1] = pDst[FFT_CALC_SIZE];
	
	*pDst++ *= invMag1;
	*pDst++ *= invMag1;
	for (uint32_t i = 1; i < FFT_CALC_SIZE / 2; ++i)
	{
		float32_t real = pDst[0], imag = pDst[1], mag, pha;
		arm_sqrt_f32(real * real + imag * imag, &mag);
		arm_atan2_f32(imag, real, &pha);
		*pDst++ = mag * invMag2;
		*pDst++ = pha * invPha;
	}
}
