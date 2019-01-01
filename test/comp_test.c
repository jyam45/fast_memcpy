#include "fast_memcpy.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"

#define NDIM 1024*1024*8

int main( int argc, char** argv ){

	//int w[NDIM],x[NDIM],y[NDIM],z[NDIM];

	int *w,*x,*y,*z;

	double t1,t2,t3,t4;

	size_t m = NDIM*sizeof(int);

	int i;

	w = (int*) malloc( NDIM * sizeof(int) );
	x = (int*) malloc( NDIM * sizeof(int) );

	for( i=0; i<NDIM; i++ ){ w[i] = i; } 

	t1 = get_realtime();
	memcpy( x, w, NDIM*sizeof(int) );
	t2 = get_realtime();

	for( i=0; i<NDIM; i++ ){
		if( x[i] != w[i] ){
			printf("entry %d-th error : w=%d, x=%d\n",i,w[i],x[i]);
			break;
		}
	}

	free(w);
	free(x);

	y = (int*) malloc( NDIM * sizeof(int) );
	z = (int*) malloc( NDIM * sizeof(int) );

	for( i=0; i<NDIM; i++ ){ y[i] = i; }

	t3 = get_realtime();
	fast_memcpy( z, y, NDIM*sizeof(int) );
	t4 = get_realtime();

	for( i=0; i<NDIM; i++ ){
		if( y[i] != z[i] ){
			printf("entry %d-th error : y=%d, z=%d\n",i,y[i],z[i]);
			break;
		}
	}

	free(y);
	free(z);


	printf("ELAPS TIME %zu bytes : memcpy %G sec ( %G B/s ), fast_memcpy %G sec ( %G B/s )\n", m, t2-t1, m/(t2-t1), t4-t3, m/(t4-t3) );

	return 0;
}

