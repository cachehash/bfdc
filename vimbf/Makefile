.PHONY: clean all

CFLAGS=-Ofast -Wno-unused-result



all: mandelbrot
	./$<

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@

%.c: %.bf compiler.out
	./compiler.out < $< > $@

compiler.c: dbf2c.bf
	./vimbf $< $< +'normal k' +'w $@' +':qa!'
	echo '}' >> $@
compiler.out: compiler.c
	$(CC) $(CFLAGS) $< -o $@

clean::
	rm -vf *.c *.out mandelbrot
