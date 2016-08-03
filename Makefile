CC=clang
MIN_OSX=10.9
DYLIB=librdata.dylib
PREFIX=/usr/local

.PHONY: test

all:
	@mkdir -p obj
	$(CC) -DHAVE_LZMA=1 -Os src/*.c -dynamiclib -o obj/$(DYLIB) -I/usr/local/include -L/usr/local/lib -llzma -lz -Wall -Werror -mmacosx-version-min=$(MIN_OSX)

install:
	cp obj/$(DYLIB) $(PREFIX)/lib/
	cp src/rdata.h $(PREFIX)/include/

uninstall:
	rm $(PREFIX)/lib/$(DYLIB)
	rm $(PREFIX)/include/rdata.h

clean:
	rm -rf obj
