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

const float32_t invMag = 1.0f / FFT_CALC_SIZE;
const float32_t invPha = 1.0f / PI;

void BSP_FFT_f32(const float32_t *pSrc, float32_t *pDst)
{
	float32_t *pTmp = pDst;
	for (uint32_t i = 0; i < FFT_CALC_SIZE; ++i)
		*pTmp++ = *pSrc++;
	
	arm_cfft_f32(&arm_cfft_sR_f32, pDst, 0, 1);
	
	float32_t sum  = pDst[0] + pDst[1];
	float32_t diff = pDst[0] - pDst[1];
	pDst[0] = sum  * invMag;
	pDst[1] = diff * invMag;
	
	const float32_t *ptw = twiddleCoef_rfft;
	      float32_t *pxA = pDst;
	      float32_t *pxB = pDst + FFT_CALC_SIZE;
	for (uint32_t i = 1; i <= FFT_CALC_SIZE / 4; ++i)
	{
		ptw += 2, pxA += 2, pxB -= 2;
		
		float32_t twS = ptw[0];  float32_t twC = ptw[1];
		float32_t xAR = pxA[0];  float32_t xAI = pxA[1];
		float32_t xBR = pxB[0];  float32_t xBI = pxB[1];
		
		float32_t xRp = xAR + xBR;
		float32_t xIm = xAI - xBI;
		float32_t xRm = xAR - xBR;
		float32_t xIp = xAI + xBI;
		
		float32_t twR = xIp * twC - xRm * twS;
		float32_t twI = xRm * twC + xIp * twS;
		
		float32_t real, imag, mag, pha;
		
		real =   xRp + twR;
		imag =   xIm - twI;
		arm_sqrt_f32(real * real + imag * imag, &mag);
		arm_atan2_f32(imag, real, &pha);
		pxA[0] = mag * invMag;
		pxA[1] = pha * invPha;
		
		real =   xRp - twR;
		imag = - xIm - twI;
		arm_sqrt_f32(real * real + imag * imag, &mag);
		arm_atan2_f32(imag, real, &pha);
		pxB[0] = mag * invMag;
		pxB[1] = pha * invPha;
	}
}
