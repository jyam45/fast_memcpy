all : unit_test comp_test perf_test

unit_test :
	gcc -O3 -o unit_test -I./src -I./test test/unit_test.c src/fast_memcpy.c

comp_test :
	gcc -O3 -o comp_test -I./src -I./test test/comp_test.c src/fast_memcpy.c -L./test -ltimer

perf_test :
	gcc -O3 -o perf_test -I./src -I./test test/perf_test.c src/fast_memcpy.c -L./test -ltimer
