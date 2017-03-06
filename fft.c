
#include "types.h"

#include "math.h"


const float sin_tb[] = {  // 精度(PI PI/2 PI/4 PI/8 PI/16 ... PI/(2^k))
	0.000000, 1.000000, 0.707107, 0.382683, 0.195090, 0.098017, 
	0.049068, 0.024541, 0.012272, 0.006136, 0.003068, 0.001534, 
	0.000767, 0.000383, 0.000192, 0.000096, 0.000048, 0.000024, 
	0.000012, 0.000006, 0.000003 
};


const float cos_tb[] = {  // 精度(PI PI/2 PI/4 PI/8 PI/16 ... PI/(2^k))
	-1.000000, 0.000000, 0.707107, 0.923880, 0.980785, 0.995185, 
	0.998795, 0.999699, 0.999925, 0.999981, 0.999995, 0.999999, 
	1.000000, 1.000000, 1.000000, 1.000000 , 1.000000, 1.000000, 
	1.000000, 1.000000, 1.000000
};


/*
 * @brief   计算二进制表示中1出现的个数的快速算法.
 *          有c个1，则循环c次
 * @inputs  
 * @outputs 
 * @retval  
 */
S32 ones_32(U32 n)
{
    U32 c =0 ;
    for (c = 0; n; ++c)
    {
        n &= (n -1) ; // 清除最低位的1
    }
    return c ;
}


/*
 * @brief   
 *   floor{long2(x)}
 *   x must > 0
 * @inputs  
 * @outputs 
 * @retval  
 */
U32 floor_log2_32(U32 x)
{
	x |= (x>>1);
	x |= (x>>2);
	x |= (x>>4);
	x |= (x>>8);
	x |= (x>>16);

	return (ones_32(x>>1));
}


/*
 * FFT Algorithm
 * === Inputs ===
 * x : complex numbers
 * N : nodes of FFT. @N should be power of 2, that is 2^(*)
 * === Output ===
 * the @x contains the result of FFT algorithm, so the original data
 * in @x is destroyed, please store them before using FFT.
 */
S32 fft(COMPLEX *x, U32 N)
{
	S32 i,j,l,k,ip;
	static U32 M = 0;
	static S32 le,le2;
	static float sR,sI,tR,tI,uR,uI;

	M = floor_log2_32(N);

	/*
	 * bit reversal sorting
	 */
	l = N >> 1;
	j = l;
	ip = N-2;
	for (i=1; i<=ip; i++) {
		if (i < j) {
			tR = x[j].real;
			tI = x[j].imag;
			x[j].real = x[i].real;
			x[j].imag = x[i].imag;
			x[i].real = tR;
			x[i].imag = tI;
		}
		k = l;
		while (k <= j) {
			j = j - k;
			k = k >> 1;
		}
		j = j + k;
	}

	/*
	 * For Loops
	 */
	for (l=1; l<=M; l++) {   /* loop for ceil{log2(N)} */
		//le = (S32)pow(2,l);
		le  = (S32)(1 << l);
		le2 = (S32)(le >> 1);
		uR = 1;
		uI = 0;

		k = floor_log2_32(le2);
		sR = cos_tb[k]; //cos(PI / le2);
		sI = -sin_tb[k];  // -sin(PI / le2)
		for (j=1; j<=le2; j++) {   /* loop for each sub DFT */
			//jm1 = j - 1;
			for (i=j-1; i<N; i+=le) {  /* loop for each butterfly */
				ip = i + le2;
				tR = x[ip].real * uR - x[ip].imag * uI;
				tI = x[ip].real * uI + x[ip].imag * uR;
				x[ip].real = x[i].real - tR;
				x[ip].imag = x[i].imag - tI;
				x[i].real += tR;
				x[i].imag += tI;
			}  /* Next i */
			tR = uR;
			uR = tR * sR - uI * sI;
			uI = tR * sI + uI *sR;
		} /* Next j */
	} /* Next l */

	return 0;
}


/*
 * Inverse FFT Algorithm
 * === Inputs ===
 * x : complex numbers
 * N : nodes of FFT. @N should be power of 2, that is 2^(*)
 * === Output ===
 * the @x contains the result of FFT algorithm, so the original data
 * in @x is destroyed, please store them before using FFT.
 */
S32 ifft(COMPLEX *x, U32 N)
{
	S32 k = 0;

	for (k=0; k<=N-1; k++) {
		x[k].imag = -x[k].imag;
	}

	fft(x, N);    /* using FFT */

	for (k=0; k<=N-1; k++) {
		x[k].real = x[k].real / N;
		x[k].imag = -x[k].imag / N;
	}

	return 0;
}

#ifndef PI
#define PI 3.141592653
#endif

U32 complex_zero(COMPLEX* x)
{
	if (x->real<0.0001 && x->real>-0.0001 && x->imag<0.0001 && x->imag>-0.0001)
		return 1;
	return 0;
}

U32 complex_trunc(COMPLEX x[], U32 num, U32 param)
{
	U32 i,j;

	for (i=0; i<num; i++)
	{
		if (!complex_zero(&x[0]))
		{
			return 1;
		}
		else
		{
			for (j=0; j<num-1; j++)
			{
				x[j] = x[j+1];
			}
			x[num-1].real = 0;
			x[num-1].imag = 0;
		}
	}
	return 0;
}

void freq_small(COMPLEX x[], U32 num, U32 param)
{
	U32 i;
#if 1
	while (param--)
	{
		for (i=num-1; i>0; i--)
		{
			x[i].real = x[i-1].real;
			x[i].imag = x[i-1].imag;
		}
		x[0].real = 0;
		x[0].imag = 0;
	}
#else
	if (param > num)
		return;
	for (i=num-1; i>num-param; i--)
	{
		x[i].real = x[i-1].real;
		x[i].imag = x[i-1].imag;
	}
	for (i=0; i<param; i++)
	{
		x[i].real = 0;
		x[i].imag = 0;
	}
#endif
}

void freq_large(COMPLEX x[], U32 num, U32 param)
{
	
}

void main()
{
	U32 i=0;
	const U32 NUM=128;
	COMPLEX x[NUM];
	memset(x, 0, sizeof(x));

	for (i=0; i<NUM; i++)
	{
		x[i].real=cos(PI * i / 8);

		//if (x[i].real<0)
		//	x[i].real=x[i].real*-1;
		
		x[i].imag=0;
	}

	for (i=0; i<NUM; i++){ HPRINTF("%f,%f\n", x[i].real, x[i].imag);} HPRINTF("--------------\n");

	fft(x, NUM);

	//for (i=0; i<NUM; i++){HPRINTF("%f,%f\n", x[i].real, x[i].imag);} HPRINTF("--------------\n");
	freq_small(x, NUM, 8);
	//for (i=0; i<NUM; i++){HPRINTF("%f,%f\n", x[i].real, x[i].imag);} HPRINTF("--------------\n");

	ifft(x,NUM);
	
	for (i=0; i<NUM; i++){HPRINTF("%f,%f\n", x[i].real, x[i].imag);} HPRINTF("--------------\n");
}
