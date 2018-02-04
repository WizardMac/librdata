## Set this to 1 if you have libFuzzer support
HAVE_FUZZER=0
## Set to 1 if you have liblzma (used for reading compressed files)
HAVE_LZMA=0

## detect operating system
ifeq ($(OS), Windows_NT)
	UNAME := Windows
else
	UNAME := $(shell uname -s)
endif

## common..
PREFIX=/usr/local

## on macOS ...
ifeq ($(UNAME), Darwin)
	CC=clang
	MIN_OSX=10.10
	DYLIB=librdata.dylib
	CFLAGS=-DHAVE_LZMA=$(HAVE_LZMA) -Wall -Werror -mmacosx-version-min=$(MIN_OSX)
	LFLAGS=-Os -dynamiclib
	BASE_LIBS=-L/usr/local/lib -lz -lrdata
endif

## on Linux ...
ifeq ($(UNAME), Linux)
	CFLAGS=-fPIC -DHAVE_LZMA=$(HAVE_LZMA) -Wall -Werror -I/usr/local/include
	LFLAGS=-Os -shared
	BASE_LIBS=-L/usr/local/lib -lz -lrdata
	DYLIB=librdata.so
endif

ifeq ($(HAVE_LZMA), 1)
	LIBS=$(BASE_LIBS) -llzma
else
	LIBS=$(BASE_LIBS)
endif


sources := 	$(wildcard src/*.c)
objects := 	$(sources:.c=.o)

.PHONY:		test

all:		library writeEx readEx

${objects}:	${sources}

library:	${objects}
	@if [ ! -d obj ]; then mkdir -p obj; fi
	$(CC) $(LFLAGS) ${objects} -o obj/$(DYLIB) $(LIBS)
	@echo "## NB: library has been built, you may need 'sudo make install' now"
ifeq ($(HAVE_FUZZER), 1)
	$(CC) -DHAVE_LZMA=1 -g src/*.c src/fuzz/fuzz_rdata.c -o obj/fuzz_rdata \
		-lstdc++ -lFuzzer $(LIBS) \
		-fsanitize=address -fsanitize-coverage=trace-pc-guard,trace-cmp \
		-Wall -Werror
endif

writeEx:	writeEx.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

readEx:		readEx.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

install:
	cp obj/$(DYLIB) $(PREFIX)/lib/
ifeq ($(UNAME), Linux)
	ldconfig
endif
	cp src/rdata.h $(PREFIX)/include/

uninstall:
	rm $(PREFIX)/lib/$(DYLIB)
	rm $(PREFIX)/include/rdata.h

clean:
	rm -rf obj/ ${objects} readEx writeEx somewhere.rdata
