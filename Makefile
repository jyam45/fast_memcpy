all : libtimer.a unit_test comp_test perf_test

libtimer.a:
	tar jxvf libtimer-1.00.tar.bz2
	cd libtimer-1.00/; ./configure; make

unit_test :
	gcc -O3 -o unit_test -I./src -I./test test/unit_test.c src/fast_memcpy.c

comp_test :
	gcc -O3 -o comp_test -I./src -I./test -I./libtimer-1.00/src test/comp_test.c src/fast_memcpy.c -L./libtimer-1.00/src -ltimer

perf_test :
	gcc -O3 -o perf_test -I./src -I./test -I./libtimer-1.00/src test/perf_test.c src/fast_memcpy.c -L./libtimer-1.00/src -ltimer
