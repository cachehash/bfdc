.PRECIOUS: %.b
.PHONY: clean all bfdc_fast debug

objs=parse.o lexer.o main.o optimize.o interp.o interp_raw.o comp.o dynarec.o
override CFLAGS := -Ofast $(CFLAGS)

LDLIBS=-lhash -Lhash

all: bfdc_fast

bfdc_fast:
	make -j bfdc

debug:
	make -j bfdc CFLAGS='-O0 -g'

%.c: %.b bfdc_fast
	./bfdc $(BFFLAGS) $< -o $@
%.b: samples/%.b
	cp $< $@

bfdc: hash/libhash.a
bfdc: $(objs)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(objs) $(LOADLIBES) $(LDLIBS) -o $@

hash/libhash.a:
	cd hash && make

clean::
	for f in *.b ; do rm -vf "$${f%.b}" ; done
	rm -vf bfdc parse.c lexer.c a.c a.out a *.o *.b
	cd hash && make clean
