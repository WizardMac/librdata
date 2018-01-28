CC=clang
MIN_OSX=10.10
DYLIB=librdata.dylib
PREFIX=/usr/local

.PHONY: test

all:
	@mkdir -p obj
	$(CC) -DHAVE_LZMA=1 -Os src/*.c -dynamiclib -o obj/$(DYLIB) -I/usr/local/include -L/usr/local/lib -llzma -lz -Wall -Werror -mmacosx-version-min=$(MIN_OSX)
	$(CC) -DHAVE_LZMA=1 -g src/*.c src/fuzz/fuzz_rdata.c -o obj/fuzz_rdata \
		-I/usr/local/include -L/usr/local/lib \
		-lstdc++ -lFuzzer -llzma -lz \
		-fsanitize=address -fsanitize-coverage=trace-pc-guard,trace-cmp \
		-Wall -Werror -mmacosx-version-min=$(MIN_OSX)

install:
	cp obj/$(DYLIB) $(PREFIX)/lib/
	cp src/rdata.h $(PREFIX)/include/

uninstall:
	rm $(PREFIX)/lib/$(DYLIB)
	rm $(PREFIX)/include/rdata.h

clean:
	rm -rf obj
