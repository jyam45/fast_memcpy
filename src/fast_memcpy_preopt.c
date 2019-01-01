#include <stdlib.h>
#include <stdio.h>
#define ALIGN_SIZE  32  // bytes for XMM
#define ALIGN_CHECK 0x1f // 1111

void* fast_memcpy(void*restrict dst, const void* restrict src, size_t n){

	const unsigned char* x = (const unsigned char*) src;
	unsigned char*       y = (unsigned char*)       dst;

	size_t i=n;
	size_t k=0;

	const unsigned char* xx;
	unsigned char*       yy;

	/* alignment check 
		align equal to 
			0) dist and src are aligned.
			1) dist is aligned, and src is unaligned.
			2) dist is unaligned, and src is aligned.
			3) dist and src are unaligned.
	*/
	uint64_t xalign = ((uint64_t)x)&ALIGN_CHECK;
	uint64_t yalign = ((uint64_t)y)&ALIGN_CHECK;

	unsigned int     align = ( ( ( (yalign>0)?1:0) << 1 ) | ((xalign>0)?1:0) );

	//printf("x aligned = %llu\n",xalign);
	//printf("y aligned = %llu\n",yalign);
	//printf("algin = %u, x=%p, y=%p\n",align,x,y);

	/* Unroll Size
		In a loop, max size of copy is 512-byte that is YMM register ( 256-bit = 32-byte ) x 16.
	*/

	if( xalign == yalign ){
		k = xalign;
		while( k > 0 ){
			*y = *x;
			x++;
			y++;
			k--;
		}
		i-= xalign;
		align = 0;
	}

	//printf("algin = %u, x=%p, y=%p\n",align,x,y);


	if( align == 0 ){

		xx = x + 512;
		yy = y + 512;
		/****************************************************************************************
		  CASE 0 : DIST and SRC are aligned
		*****************************************************************************************/
		/* 512-bytes block */
		while( i > 512 ){

			//printf("i=%zu\n",i);
			__asm__ __volatile__(
					"\n\t"
					"vmovdqa -16*32(%[x]), %%ymm0 \n\t"
					"vmovdqa -15*32(%[x]), %%ymm1 \n\t"
					"vmovdqa -14*32(%[x]), %%ymm2 \n\t"
					"vmovdqa -13*32(%[x]), %%ymm3 \n\t"
					"vmovdqa -12*32(%[x]), %%ymm4 \n\t"
					"vmovdqa -11*32(%[x]), %%ymm5 \n\t"
					"vmovdqa -10*32(%[x]), %%ymm6 \n\t"
					"vmovdqa  -9*32(%[x]), %%ymm7 \n\t"
					"vmovdqa  -8*32(%[x]), %%ymm8 \n\t"
					"vmovdqa  -7*32(%[x]), %%ymm9 \n\t"
					"vmovdqa  -6*32(%[x]), %%ymm10\n\t"
					"vmovdqa  -5*32(%[x]), %%ymm11\n\t"
					"vmovdqa  -4*32(%[x]), %%ymm12\n\t"
					"vmovdqa  -3*32(%[x]), %%ymm13\n\t"
					"vmovdqa  -2*32(%[x]), %%ymm14\n\t"
					"vmovdqa  -1*32(%[x]), %%ymm15\n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,  -16*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,  -15*32(%[y])\n\t"
					"vmovdqa %%ymm2  ,  -14*32(%[y])\n\t"
					"vmovdqa %%ymm3  ,  -13*32(%[y])\n\t"
					"vmovdqa %%ymm4  ,  -12*32(%[y])\n\t"
					"vmovdqa %%ymm5  ,  -11*32(%[y])\n\t"
					"vmovdqa %%ymm6  ,  -10*32(%[y])\n\t"
					"vmovdqa %%ymm7  ,   -9*32(%[y])\n\t"
					"vmovdqa %%ymm8  ,   -8*32(%[y])\n\t"
					"vmovdqa %%ymm9  ,   -7*32(%[y])\n\t"
					"vmovdqa %%ymm10 ,   -6*32(%[y])\n\t"
					"vmovdqa %%ymm11 ,   -5*32(%[y])\n\t"
					"vmovdqa %%ymm12 ,   -4*32(%[y])\n\t"
					"vmovdqa %%ymm13 ,   -3*32(%[y])\n\t"
					"vmovdqa %%ymm14 ,   -2*32(%[y])\n\t"
					"vmovdqa %%ymm15 ,   -1*32(%[y])\n\t"
					"\n\t"
					"subq $-16*32, %[x]\n\t"
					"subq $-16*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(xx),[y]"=r"(yy)
					:"0"(xx),"1"(yy)
					);
			i-=512;
		}
		/* 256-bytes block */
		if( i > 256 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa -16*32(%[x]), %%ymm0 \n\t"
					"vmovdqa -15*32(%[x]), %%ymm1 \n\t"
					"vmovdqa -14*32(%[x]), %%ymm2 \n\t"
					"vmovdqa -13*32(%[x]), %%ymm3 \n\t"
					"vmovdqa -12*32(%[x]), %%ymm4 \n\t"
					"vmovdqa -11*32(%[x]), %%ymm5 \n\t"
					"vmovdqa -10*32(%[x]), %%ymm6 \n\t"
					"vmovdqa  -9*32(%[x]), %%ymm7 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,  -16*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,  -15*32(%[y])\n\t"
					"vmovdqa %%ymm2  ,  -14*32(%[y])\n\t"
					"vmovdqa %%ymm3  ,  -13*32(%[y])\n\t"
					"vmovdqa %%ymm4  ,  -12*32(%[y])\n\t"
					"vmovdqa %%ymm5  ,  -11*32(%[y])\n\t"
					"vmovdqa %%ymm6  ,  -10*32(%[y])\n\t"
					"vmovdqa %%ymm7  ,   -9*32(%[y])\n\t"
					"\n\t"
					"subq $-8*32, %[x]\n\t"
					"subq $-8*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(xx),[y]"=r"(yy)
					:"0"(xx),"1"(yy)
					);
			i-=256;
		}
		/* 128-bytes block */
		if( i > 128 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa -16*32(%[x]), %%ymm0 \n\t"
					"vmovdqa -15*32(%[x]), %%ymm1 \n\t"
					"vmovdqa -14*32(%[x]), %%ymm2 \n\t"
					"vmovdqa -13*32(%[x]), %%ymm3 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,  -16*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,  -15*32(%[y])\n\t"
					"vmovdqa %%ymm2  ,  -14*32(%[y])\n\t"
					"vmovdqa %%ymm3  ,  -13*32(%[y])\n\t"
					"\n\t"
					"subq $-4*32, %[x]\n\t"
					"subq $-4*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(xx),[y]"=r"(yy)
					:"0"(xx),"1"(yy)
					);
			i-=128;
		}
		/* 64-bytes block */
		if( i > 64 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa -16*32(%[x]), %%ymm0 \n\t"
					"vmovdqa -15*32(%[x]), %%ymm1 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,  -16*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,  -15*32(%[y])\n\t"
					"\n\t"
					"subq $-2*32, %[x]\n\t"
					"subq $-2*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(xx),[y]"=r"(yy)
					:"0"(xx),"1"(yy)
					);

			i-=64;
		}
		/* 32-bytes block */
		if( i > 32 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa -16*32(%[x]), %%ymm0 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,  -16*32(%[y])\n\t"
					"\n\t"
					"subq $-1*32, %[x]\n\t"
					"subq $-1*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(xx),[y]"=r"(yy)
					:"0"(xx),"1"(yy)
					);

			i-=32;
		}

		x = xx-512;
		y = yy-512;

	}else if ( align == 1 ){

		/****************************************************************************************
		  CASE 1 : DIST is aligned, SRC is unaligned
		*****************************************************************************************/
		/* 512-bytes block */
		while( i > 512 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqu  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqu  3*32(%[x]), %%ymm3 \n\t"
					"vmovdqu  4*32(%[x]), %%ymm4 \n\t"
					"vmovdqu  5*32(%[x]), %%ymm5 \n\t"
					"vmovdqu  6*32(%[x]), %%ymm6 \n\t"
					"vmovdqu  7*32(%[x]), %%ymm7 \n\t"
					"vmovdqu  8*32(%[x]), %%ymm8 \n\t"
					"vmovdqu  9*32(%[x]), %%ymm9 \n\t"
					"vmovdqu 10*32(%[x]), %%ymm10\n\t"
					"vmovdqu 11*32(%[x]), %%ymm11\n\t"
					"vmovdqu 12*32(%[x]), %%ymm12\n\t"
					"vmovdqu 13*32(%[x]), %%ymm13\n\t"
					"vmovdqu 14*32(%[x]), %%ymm14\n\t"
					"vmovdqu 15*32(%[x]), %%ymm15\n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqa %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqa %%ymm3  ,   3*32(%[y])\n\t"
					"vmovdqa %%ymm4  ,   4*32(%[y])\n\t"
					"vmovdqa %%ymm5  ,   5*32(%[y])\n\t"
					"vmovdqa %%ymm6  ,   6*32(%[y])\n\t"
					"vmovdqa %%ymm7  ,   7*32(%[y])\n\t"
					"vmovdqa %%ymm8  ,   8*32(%[y])\n\t"
					"vmovdqa %%ymm9  ,   9*32(%[y])\n\t"
					"vmovdqa %%ymm10 ,  10*32(%[y])\n\t"
					"vmovdqa %%ymm11 ,  11*32(%[y])\n\t"
					"vmovdqa %%ymm12 ,  12*32(%[y])\n\t"
					"vmovdqa %%ymm13 ,  13*32(%[y])\n\t"
					"vmovdqa %%ymm14 ,  14*32(%[y])\n\t"
					"vmovdqa %%ymm15 ,  15*32(%[y])\n\t"
					"\n\t"
					"subq $-16*32, %[x]\n\t"
					"subq $-16*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=512;
		}
		/* 256-bytes block */
		if( i > 256 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqu  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqu  3*32(%[x]), %%ymm3 \n\t"
					"vmovdqu  4*32(%[x]), %%ymm4 \n\t"
					"vmovdqu  5*32(%[x]), %%ymm5 \n\t"
					"vmovdqu  6*32(%[x]), %%ymm6 \n\t"
					"vmovdqu  7*32(%[x]), %%ymm7 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqa %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqa %%ymm3  ,   3*32(%[y])\n\t"
					"vmovdqa %%ymm4  ,   4*32(%[y])\n\t"
					"vmovdqa %%ymm5  ,   5*32(%[y])\n\t"
					"vmovdqa %%ymm6  ,   6*32(%[y])\n\t"
					"vmovdqa %%ymm7  ,   7*32(%[y])\n\t"
					"\n\t"
					"subq $-8*32, %[x]\n\t"
					"subq $-8*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=256;
		}
		/* 128-bytes block */
		if( i > 128 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqu  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqu  3*32(%[x]), %%ymm3 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqa %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqa %%ymm3  ,   3*32(%[y])\n\t"
					"\n\t"
					"subq $-4*32, %[x]\n\t"
					"subq $-4*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=128;
		}
		/* 64-bytes block */
		if( i > 64 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqa %%ymm1  ,   1*32(%[y])\n\t"
					"\n\t"
					"subq $-2*32, %[x]\n\t"
					"subq $-2*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);

			i-=64;
		}
		/* 32-bytes block */
		if( i > 32 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"\n\t"
					"vmovdqa %%ymm0  ,   0*32(%[y])\n\t"
					"\n\t"
					"subq $-1*32, %[x]\n\t"
					"subq $-1*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);

			i-=32;
		}

	}else if ( align == 2 ){

		/****************************************************************************************
		  CASE 2 : DIST is unaligned, SRC is aligned
		*****************************************************************************************/
		/* 512-bytes block */
		while( i > 512 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqa  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqa  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqa  3*32(%[x]), %%ymm3 \n\t"
					"vmovdqa  4*32(%[x]), %%ymm4 \n\t"
					"vmovdqa  5*32(%[x]), %%ymm5 \n\t"
					"vmovdqa  6*32(%[x]), %%ymm6 \n\t"
					"vmovdqa  7*32(%[x]), %%ymm7 \n\t"
					"vmovdqa  8*32(%[x]), %%ymm8 \n\t"
					"vmovdqa  9*32(%[x]), %%ymm9 \n\t"
					"vmovdqa 10*32(%[x]), %%ymm10\n\t"
					"vmovdqa 11*32(%[x]), %%ymm11\n\t"
					"vmovdqa 12*32(%[x]), %%ymm12\n\t"
					"vmovdqa 13*32(%[x]), %%ymm13\n\t"
					"vmovdqa 14*32(%[x]), %%ymm14\n\t"
					"vmovdqa 15*32(%[x]), %%ymm15\n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqu %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqu %%ymm3  ,   3*32(%[y])\n\t"
					"vmovdqu %%ymm4  ,   4*32(%[y])\n\t"
					"vmovdqu %%ymm5  ,   5*32(%[y])\n\t"
					"vmovdqu %%ymm6  ,   6*32(%[y])\n\t"
					"vmovdqu %%ymm7  ,   7*32(%[y])\n\t"
					"vmovdqu %%ymm8  ,   8*32(%[y])\n\t"
					"vmovdqu %%ymm9  ,   9*32(%[y])\n\t"
					"vmovdqu %%ymm10 ,  10*32(%[y])\n\t"
					"vmovdqu %%ymm11 ,  11*32(%[y])\n\t"
					"vmovdqu %%ymm12 ,  12*32(%[y])\n\t"
					"vmovdqu %%ymm13 ,  13*32(%[y])\n\t"
					"vmovdqu %%ymm14 ,  14*32(%[y])\n\t"
					"vmovdqu %%ymm15 ,  15*32(%[y])\n\t"
					"\n\t"
					"subq $-16*32, %[x]\n\t"
					"subq $-16*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=512;
		}
		/* 256-bytes block */
		if( i > 256 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqa  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqa  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqa  3*32(%[x]), %%ymm3 \n\t"
					"vmovdqa  4*32(%[x]), %%ymm4 \n\t"
					"vmovdqa  5*32(%[x]), %%ymm5 \n\t"
					"vmovdqa  6*32(%[x]), %%ymm6 \n\t"
					"vmovdqa  7*32(%[x]), %%ymm7 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqu %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqu %%ymm3  ,   3*32(%[y])\n\t"
					"vmovdqu %%ymm4  ,   4*32(%[y])\n\t"
					"vmovdqu %%ymm5  ,   5*32(%[y])\n\t"
					"vmovdqu %%ymm6  ,   6*32(%[y])\n\t"
					"vmovdqu %%ymm7  ,   7*32(%[y])\n\t"
					"\n\t"
					"subq $-8*32, %[x]\n\t"
					"subq $-8*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=256;
		}
		/* 128-bytes block */
		if( i > 128 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqa  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqa  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqa  3*32(%[x]), %%ymm3 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqu %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqu %%ymm3  ,   3*32(%[y])\n\t"
					"\n\t"
					"subq $-4*32, %[x]\n\t"
					"subq $-4*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=128;
		}
		/* 64-bytes block */
		if( i > 64 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqa  1*32(%[x]), %%ymm1 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"\n\t"
					"subq $-2*32, %[x]\n\t"
					"subq $-2*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);

			i-=64;
		}
		/* 32-bytes block */
		if( i > 32 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqa  0*32(%[x]), %%ymm0 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"\n\t"
					"subq $-1*32, %[x]\n\t"
					"subq $-1*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);

			i-=32;
		}

	}else {

		/****************************************************************************************
		  CASE 3 : DIST and SRC are unaligned
		*****************************************************************************************/
		/* 512-bytes block */
		while( i > 512 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqu  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqu  3*32(%[x]), %%ymm3 \n\t"
					"vmovdqu  4*32(%[x]), %%ymm4 \n\t"
					"vmovdqu  5*32(%[x]), %%ymm5 \n\t"
					"vmovdqu  6*32(%[x]), %%ymm6 \n\t"
					"vmovdqu  7*32(%[x]), %%ymm7 \n\t"
					"vmovdqu  8*32(%[x]), %%ymm8 \n\t"
					"vmovdqu  9*32(%[x]), %%ymm9 \n\t"
					"vmovdqu 10*32(%[x]), %%ymm10\n\t"
					"vmovdqu 11*32(%[x]), %%ymm11\n\t"
					"vmovdqu 12*32(%[x]), %%ymm12\n\t"
					"vmovdqu 13*32(%[x]), %%ymm13\n\t"
					"vmovdqu 14*32(%[x]), %%ymm14\n\t"
					"vmovdqu 15*32(%[x]), %%ymm15\n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqu %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqu %%ymm3  ,   3*32(%[y])\n\t"
					"vmovdqu %%ymm4  ,   4*32(%[y])\n\t"
					"vmovdqu %%ymm5  ,   5*32(%[y])\n\t"
					"vmovdqu %%ymm6  ,   6*32(%[y])\n\t"
					"vmovdqu %%ymm7  ,   7*32(%[y])\n\t"
					"vmovdqu %%ymm8  ,   8*32(%[y])\n\t"
					"vmovdqu %%ymm9  ,   9*32(%[y])\n\t"
					"vmovdqu %%ymm10 ,  10*32(%[y])\n\t"
					"vmovdqu %%ymm11 ,  11*32(%[y])\n\t"
					"vmovdqu %%ymm12 ,  12*32(%[y])\n\t"
					"vmovdqu %%ymm13 ,  13*32(%[y])\n\t"
					"vmovdqu %%ymm14 ,  14*32(%[y])\n\t"
					"vmovdqu %%ymm15 ,  15*32(%[y])\n\t"
					"\n\t"
					"subq $-16*32, %[x]\n\t"
					"subq $-16*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=512;
		}
		/* 256-bytes block */
		if( i > 256 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqu  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqu  3*32(%[x]), %%ymm3 \n\t"
					"vmovdqu  4*32(%[x]), %%ymm4 \n\t"
					"vmovdqu  5*32(%[x]), %%ymm5 \n\t"
					"vmovdqu  6*32(%[x]), %%ymm6 \n\t"
					"vmovdqu  7*32(%[x]), %%ymm7 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqu %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqu %%ymm3  ,   3*32(%[y])\n\t"
					"vmovdqu %%ymm4  ,   4*32(%[y])\n\t"
					"vmovdqu %%ymm5  ,   5*32(%[y])\n\t"
					"vmovdqu %%ymm6  ,   6*32(%[y])\n\t"
					"vmovdqu %%ymm7  ,   7*32(%[y])\n\t"
					"\n\t"
					"subq $-8*32, %[x]\n\t"
					"subq $-8*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=256;
		}
		/* 128-bytes block */
		if( i > 128 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"vmovdqu  2*32(%[x]), %%ymm2 \n\t"
					"vmovdqu  3*32(%[x]), %%ymm3 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"vmovdqu %%ymm2  ,   2*32(%[y])\n\t"
					"vmovdqu %%ymm3  ,   3*32(%[y])\n\t"
					"\n\t"
					"subq $-4*32, %[x]\n\t"
					"subq $-4*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);
			i-=128;
		}
		/* 64-bytes block */
		if( i > 64 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"vmovdqu  1*32(%[x]), %%ymm1 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"vmovdqu %%ymm1  ,   1*32(%[y])\n\t"
					"\n\t"
					"subq $-2*32, %[x]\n\t"
					"subq $-2*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);

			i-=64;
		}
		/* 32-bytes block */
		if( i > 32 ){

			__asm__ __volatile__(
					"\n\t"
					"vmovdqu  0*32(%[x]), %%ymm0 \n\t"
					"\n\t"
					"vmovdqu %%ymm0  ,   0*32(%[y])\n\t"
					"\n\t"
					"subq $-1*32, %[x]\n\t"
					"subq $-1*32, %[y]\n\t"
					"\n\t"
					:[x]"=r"(x),[y]"=r"(y)
					:"0"(x),"1"(y)
					);

			i-=32;
		}

	}


	/* other block */
	while( i > 0 ){
		*y = *x;
		x++;
		y++;
		i--;
	}

	return dst;
}
