CC = gcc -pthread -ggdb 
CFLAGS = -Og -std=gnu11 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function

all: zad5 zad6 zad7 zad9 zad10

zad5: zad5.o

zad6: zad6.o

zad7: zad7.o

zad9: zad9.o

zad10: zad10.o


clean:
	rm -f zad5 zad6 zad7 zad9 zad10
	rm -f *.o *.so *.lo
	rm -f *~

# vim: ts=8 sw=8 noet
