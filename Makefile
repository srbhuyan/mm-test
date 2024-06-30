all: serial parallel

serial:
	gcc -std=c99 -o matmul_serial matmul_serial.c

parallel:
	gcc -std=c99 -o matmul_parallel matmul_parallel.c -lpthread

clean:
	rm matmul_serial matmul_parallel
