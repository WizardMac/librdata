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
BASE_LIBS=-L/usr/local/lib
BASE_CFLAGS=-Os -DHAVE_LZMA=$(HAVE_LZMA) -Wall -Werror -I/usr/local/include -std=c99

## on macOS ...
ifeq ($(UNAME), Darwin)
	CC=clang
	MIN_OSX=10.11
	DYLIB=librdata.dylib
	CFLAGS=$(BASE_CFLAGS) -mmacosx-version-min=$(MIN_OSX) -DHAVE_APPLE_COMPRESSION=1
	LFLAGS=-dynamiclib -mmacosx-version-min=$(MIN_OSX)
	LIBS=$(BASE_LIBS) -lcompression
endif

ifneq ($(UNAME), Darwin)
ifeq ($(HAVE_LZMA), 1)
	LIBS=$(BASE_LIBS) -z -llzma
else
	LIBS=$(BASE_LIBS) -z
endif
endif


## on Linux ...
ifeq ($(UNAME), Linux)
	DYLIB=librdata.so
	CFLAGS=$(BASE_CFLAGS) -fPIC
	LFLAGS=-shared
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
	$(CC) -Isrc $(CFLAGS) -o $@ $^ -Lobj $(LIBS) -lrdata

readEx:		readEx.c
	$(CC) -Isrc $(CFLAGS) -o $@ $^ -Lobj $(LIBS) -lrdata

install:
	cp obj/$(DYLIB) $(PREFIX)/lib/
ifeq ($(UNAME), Linux)
	ldconfig
endif
ifeq ($(UNAME), Darwin)
	install_name_tool -id $(PREFIX)/lib/$(DYLIB) $(PREFIX)/lib/$(DYLIB)
endif
	cp src/rdata.h $(PREFIX)/include/

uninstall:
	rm $(PREFIX)/lib/$(DYLIB)
	rm $(PREFIX)/include/rdata.h

clean:
	rm -rf obj/ ${objects} readEx writeEx somewhere.rdata
