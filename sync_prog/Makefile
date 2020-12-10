all: clean build

clean:
	$(RM) fifo_test

build:
	@gcc -o fifo_test tas64.S spin_lock.c sem.c fifo.c fifo_test.c -std=gnu99
