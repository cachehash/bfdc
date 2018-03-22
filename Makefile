.phony: clean all

all: mandelbrot.out
	./$<
%.out: %.c
	gcc -Ofast $< -o $@
%.c: %.b compiler.out
	./compiler.out < $< > $@
clean:
	rm *.c *.out
