
#include "types.h"

typedef struct {
	float real;
	float imag;
} COMPLEX;

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
int ones_32(U32 n)
{
	unsigned int c =0 ;
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
int fft(COMPLEX *x, U32 N)
{
	int i,j,l,k,ip;
	static U32 M = 0;
	static int le,le2;
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
		//le = (int)pow(2,l);
		le  = (int)(1 << l);
		le2 = (int)(le >> 1);
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
int ifft(COMPLEX *x, U32 N)
{
	int k = 0;

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

void main()
{
	COMPLEX x[4];
	fft(x, 4);
	ifft(x,4);  
}

