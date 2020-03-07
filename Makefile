.PRECIOUS: %.bf
.PHONY: clean all bfdc_fast debug

objs=parse.tab.o lexer.o main.o optimize.o interp.o interp_raw.o dynarec.o comp.o comp_c.o comp_go.o comp_mips.o fast.o

ifeq ($(CC),clang)
	OPT=-O2
else
	OPT=-O2 -march=native
endif

override CFLAGS := $(OPT) $(CFLAGS)
override LDLIBS := -lm -lhash -Lhash $(LDLIBS)

all: bfdc_fast

bfdc_fast:
	make -j bfdc

debug:
	make -j bfdc CFLAGS='-O0 -g'

PREFIX=/usr/local

%.c: %.bf bfdc_fast
	./bfdc $(BFFLAGS) -t c $< -o $@
%.go: %.bf bfdc_fast
	./bfdc $(BFFLAGS) -t go $< -o $@
%.spim: %.bf bfdc_fast
	./bfdc $(BFFLAGS) -t spim $< -o $@
%.mips.s: %.bf bfdc_fast
	./bfdc $(BFFLAGS) -t mips $< -o $@
%.mips: %.mips.s
	mips64-linux-gnuabi64-gcc $< -static -o $@
	

%.s: %.bf bfdc_fast
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

install: bfdc_fast
	install -m 755 bfdc $(PREFIX)/bin

clean::
	for f in *.bf ; do rm -vf "$${f%.bf}" ; done
	rm -vf bfdc parse.tab.c parse.tab.h lexer.c a.c a.out a *.o *.bf *.go *.s *.spim *.mips
	cd hash && make clean
	cd vimbf && make clean
