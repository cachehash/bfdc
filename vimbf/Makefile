.PHONY: clean all

CFLAGS=-Ofast -Wno-unused-result



all: mandelbrot
	./$<

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@

%.c: %.b compiler.out
	./compiler.out < $< > $@

compiler.c: dbf2c.b
	./vimbf $< $< +'normal k' +'w $@' +':qa!'
	echo '}' >> $@
compiler.out: compiler.c
	$(CC) $(CFLAGS) $< -o $@

clean::
	rm -vf *.c *.out
