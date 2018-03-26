.PRECIOUS: %.bf
.PHONY: clean all bfdc_fast debug

objs=parse.tab.o lexer.o main.o optimize.o interp.o interp_raw.o dynarec.o comp.o comp_c.o comp_go.o
override CFLAGS := -Ofast -march=native $(CFLAGS)

LDLIBS=-lhash -Lhash

all: bfdc_fast

bfdc_fast:
	make -j bfdc

debug:
	make -j bfdc CFLAGS='-O0 -g'

%.c: %.bf bfdc_fast
	./bfdc $(BFFLAGS) $< -o $@
%.go: %.bf bfdc_fast
	./bfdc $(BFFLAGS) $< -o $@
%.bf: samples/%.bf
	cp $< $@

lexer.o: parse.tab.h
%.tab.c %.tab.h: %.y
	bison $(YFLAGS) -d $<

bfdc: hash/libhash.a
bfdc: $(objs)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(objs) $(LOADLIBES) $(LDLIBS) -o $@

hash/libhash.a:
	cd hash && make

clean::
	for f in *.bf ; do rm -vf "$${f%.bf}" ; done
	rm -vf bfdc parse.tab.c parse.tab.h lexer.c a.c a.out a *.o *.bf *.go
	cd hash && make clean
	cd vimbf && make clean
