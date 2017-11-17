make:	
	gcc -c -fpic mem.c	
	gcc -shared -o libmem.so mem.o	
	gcc -lmem -L. -o myprogram mymain.c libmem.so	
