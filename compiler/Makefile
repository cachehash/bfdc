.PHONY: clean all

objs=parse.o lexer.o main.o optimize.o interp.o interp_raw.o comp.o dynarec.o
CFLAGS=-Ofast

LDLIBS=-lhash -Lhash

all:
	make -j bfc

%.c: %.b bfc
	./bfc $< -o $@

bfc: hash/libhash.a
bfc: $(objs)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

hash/libhash.a:
	cd hash && make

clean::
	rm -vf parse.c lexer.c bfc a.c a.out test test.c a *.o
	cd hash && make clean
