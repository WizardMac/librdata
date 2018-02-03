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

ifeq ($(UNAME), Darwin)
	CC=clang
	MIN_OSX=10.10
	DYLIB=librdata.dylib
	PREFIX=/usr/local

	CFLAGS=-DHAVE_LZMA=$(HAVE_LZMA) -Wall -Werror -dynamiclib -mmacosx-version-min=$(MIN_OSX)
	BASE_LIBS=-L/usr/local/lib -lz
endif

ifeq ($(UNAME), Linux)
	CFLAGS=-DHAVE_LZMA=$(HAVE_LZMA) -Wall -Werror -I/usr/local/include -Isrc/
	BASE_LIBS=-L/usr/local/lib -lz
endif

ifeq ($(HAVE_LZMA), 1)
	LIBS=$(BASE_LIBS) -llzma
else
	LIBS=$(BASE_LIBS)
endif

.PHONY: 	test

all:		writeEx readEx library

writeEx:	writeEx.c
	$(CC) $(CFLAGS) -o $@ $^ src/*c $(LIBS)

readEx:		readEx.c
	$(CC) $(CFLAGS) -o $@ $^ src/*c $(LIBS)

library:
ifeq ($(UNAME), Darwin)
	@mkdir -p obj
	$(CC) $(CFLAGS) -Os src/*.c -o obj/$(DYLIB) $(LIBS) 
endif
ifeq ($(HAVE_FUZZER), 1)
	$(CC) -DHAVE_LZMA=1 -g src/*.c src/fuzz/fuzz_rdata.c -o obj/fuzz_rdata \
		-lstdc++ -lFuzzer $(LIBS) \
		-fsanitize=address -fsanitize-coverage=trace-pc-guard,trace-cmp \
		-Wall -Werror 
endif

install:
	cp obj/$(DYLIB) $(PREFIX)/lib/
	cp src/rdata.h $(PREFIX)/include/

uninstall:
	rm $(PREFIX)/lib/$(DYLIB)
	rm $(PREFIX)/include/rdata.h

clean:
	rm -rf obj
	rm readEx
	rm writeEx
