CFLAGS=-Wall
FLAGS=-std=c++11 -stdlib=libc++
SRC=$(wildcard src/cobra/*/*.cc) $(wildcard src/*/*.cc)

all: $(SRC)
	g++ $(FLAGS) -c -Wall $^
	make lib

lib:
	ar rvs build/libcobra.a $(wildcard *.o)
	make cleano

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra 
	./build/Cobra ${ARGS}

testall:
	make all
	make test

cleano:
	rm -rf *.o

clean:
	rm -rf *.o
	rm -rf build/*.a

buildtest:
	make clean
	make all
	make test

mem: 
	valgrind --tool=memcheck --leak-check=yes ./build/Cobra

.PHONY: all clean test