.PHONY: clean all bfc_fast

objs=parse.o lexer.o main.o optimize.o interp.o interp_raw.o comp.o dynarec.o
CFLAGS=-Ofast

LDLIBS=-lhash -Lhash

all: bfc_fast

bfc_fast:
	make -j bfc

%.c: %.b bfc_fast
	./bfc $(BFFLAGS) $< -o $@

bfc: hash/libhash.a
bfc: $(objs)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(objs) $(LOADLIBES) $(LDLIBS) -o $@

hash/libhash.a:
	cd hash && make

clean::
	rm -vf parse.c lexer.c bfc a.c a.out test test.c a *.o mandelbrot
	cd hash && make clean
