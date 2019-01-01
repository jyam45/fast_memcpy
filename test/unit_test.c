#include "fast_memcpy.h"
#include <stdio.h>

int main( int argc, char** argv ){

	int x[1023],y[1023];

	int i;
	for( i=0; i<1023; i++ ){
		x[i] = i;
	}

	fast_memcpy( y, x, 1023*sizeof(int) );

	for( i=0; i<1023; i++ ){
		if( x[i] != y[i] ){
			printf("entry %d-th error : x=%d, y=%d\n",i,x[i],y[i]);
			break;
		}
	}

	return 0;
}

