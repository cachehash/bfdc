.PHONY: clean all
CFLAGS=-Ofast

LDLIBS=-lhash -Lhash

all: bfc

%.c: %.b bfc
	./bfc $< -o $@

bfc: hash/libhash.a
bfc: parse.o lexer.o main.o optimize.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

hash/libhash.a:
	cd hash && make

clean::
	rm -vf parse.c lexer.c bfc a.c a.out test test.c a *.o
	cd hash && make clean
