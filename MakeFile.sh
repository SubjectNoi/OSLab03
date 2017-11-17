gcc -c -fpic mem.c
gcc -shared -o libmem.so mem.o
gcc -lmem -L. -o main test.c libmem.so
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./main
