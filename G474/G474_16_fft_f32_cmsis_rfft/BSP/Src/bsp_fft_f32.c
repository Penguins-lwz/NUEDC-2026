#include "bsp_fft_f32.h"

#if FFT_CALC_SIZE == 32U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len16
	#define twiddleCoef_rfft	twiddleCoef_rfft_32
#elif FFT_CALC_SIZE == 64U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len32
	#define twiddleCoef_rfft	twiddleCoef_rfft_64
#elif FFT_CALC_SIZE == 128U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len64
	#define twiddleCoef_rfft	twiddleCoef_rfft_128
#elif FFT_CALC_SIZE == 256U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len128
	#define twiddleCoef_rfft	twiddleCoef_rfft_256
#elif FFT_CALC_SIZE == 512U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len256
	#define twiddleCoef_rfft	twiddleCoef_rfft_512
#elif FFT_CALC_SIZE == 1024U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len512
	#define twiddleCoef_rfft	twiddleCoef_rfft_1024
#elif FFT_CALC_SIZE == 2048U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len1024
	#define twiddleCoef_rfft	twiddleCoef_rfft_2048
#elif FFT_CALC_SIZE == 4096U
	#define arm_cfft_sR_f32		arm_cfft_sR_f32_len2048
	#define twiddleCoef_rfft	twiddleCoef_rfft_4096
#endif

const float32_t invMag1 = 1.0f / FFT_CALC_SIZE;
const float32_t invMag2 = 2.0f / FFT_CALC_SIZE;
const float32_t invPha  = 1.0f / PI;

static float32_t pState[FFT_CALC_SIZE];

void BSP_FFT_f32(const float32_t *pSrc, float32_t *pDst)
{
	float32_t *pTmp = pState;
	for (uint32_t i = 0; i < FFT_CALC_SIZE; ++i)
		*pTmp++ = *pSrc++;
	
	arm_rfft_fast_instance_f32 S = {
		.Sint = arm_cfft_sR_f32,
		.fftLenRFFT = FFT_CALC_SIZE,
		.pTwiddleRFFT = twiddleCoef_rfft,
	};
	arm_rfft_fast_f32(&S, pState, pDst, 0);
	
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
