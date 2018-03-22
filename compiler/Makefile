.PHONY: clean all
CFLAGS=-Ofast

all: bfc

%.c: %.b bfc
	./bfc $< -o $@

bfc: parse.c lexer.c main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -vf parse.c lexer.c bfc a.c a.out test
