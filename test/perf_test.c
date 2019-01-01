#include "fast_memcpy.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"

#define MDIM 1024*1024*1024

int main( int argc, char** argv ){

	//int w[NDIM],x[NDIM],y[NDIM],z[NDIM];

	int *w,*x,*y,*z;

	double t1,t2,t3,t4;

	size_t ndim;
	size_t m ;

	int i;

	printf("Datasize [B], memcpy time [sec], fast_memcpy time [sec], memcpy performance [B/s], fast_memcpy performance [B/s], speed-up ratio\n" );

	for( ndim = 16; ndim < MDIM; ndim=ndim*2 ){

		m = ndim * sizeof(int);

		w = (int*) malloc( ndim * sizeof(int) );
		x = (int*) malloc( ndim * sizeof(int) );

		for( i=0; i<ndim; i++ ){ w[i] = i; } 

		t1 = get_realtime();
		memcpy( x, w, ndim*sizeof(int) );
		t2 = get_realtime();

		for( i=0; i<ndim; i++ ){
			if( x[i] != w[i] ){
				printf("entry %d-th error : w=%d, x=%d\n",i,w[i],x[i]);
				break;
			}
		}

		free(w);
		free(x);

		y = (int*) malloc( ndim * sizeof(int) );
		z = (int*) malloc( ndim * sizeof(int) );

		for( i=0; i<ndim; i++ ){ y[i] = i; }

		t3 = get_realtime();
		fast_memcpy( z, y, ndim*sizeof(int) );
		t4 = get_realtime();

		for( i=0; i<ndim; i++ ){
			if( y[i] != z[i] ){
				printf("entry %d-th error : y=%d, z=%d\n",i,y[i],z[i]);
				break;
			}
		}

		free(y);
		free(z);

		printf("%zu, %G, %G, %G, %G, %G\n", m, t2-t1, t4-t3, m/(t2-t1), m/(t4-t3), (t2-t1)/(t4-t3) );

	}

	return 0;
}

