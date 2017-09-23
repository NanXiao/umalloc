all:
	cc -g -O2 -Wall -Werror -c -fpic umalloc.c
	cc -shared -o libumalloc.so umalloc.o

install:
	cp libumalloc.so /usr/local/lib
	cp umalloc.h /usr/local/include

uninstall:
	rm /usr/local/include/umalloc.h /usr/local/lib/libumalloc.so

clean:
	rm -rf umalloc.o libumalloc.so