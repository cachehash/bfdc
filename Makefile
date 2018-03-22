.phony: clean all

all: mandelbrot.out
	./$<

%.out: %.c
	gcc -Ofast $< -o $@

%.c: %.b compiler.out
	./compiler.out < $< > $@

compiler.c: dbf2c.b
	./vimbf $< $< +'normal k' +'w $@' +':qa!'
	echo '}' >> $@
compiler.out: compiler.c
	gcc -Ofast $< -o $@

clean:
	rm *.c *.out
