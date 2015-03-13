CFLAGS=-Wall
FLAGS=-std=c++11 -stdlib=libc++
SRC=$(wildcard src/*.cc)
CSRC=$(wildcard src/*.c)

c: $(CSRC)
	gcc -c -Wall $^
	make lib
	make ctest

ctest:
	gcc -I ./ test/main.c -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra ${ARGS}

all: $(SRC)
	g++ $(FLAGS) -c -Wall $^
	make lib

lib:
	ar rvs build/libcobra.a $(wildcard *.o)
	make cleano

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra 
	./build/Cobra ${ARGS}


cleano:
	rm -rf *.o

clean:
	rm -rf *.o
	rm -rf build/*.a

buildtest:
	make clean
	make all
	make test

.PHONY: all clean test