.PHONY: clean all
CFLAGS=-Ofast

LDLIBS=-lhash -Lhash

all: bfc

%.c: %.b bfc
	./bfc $< -o $@

bfc: hash/libhash.a
bfc: parse.c lexer.c main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

hash/libhash.a:
	cd hash && make

clean::
	rm -vf parse.c lexer.c bfc a.c a.out test test.c
	cd hash && make clean
