.PHONY: clean all

all: bfc

bfc: parse.c lexer.c main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -vf parse.c lexer.c bfc a.c a.out
