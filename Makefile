CC=g++
CFLAGS=-c -I src/
LDFLAGS=
SOURCES=$(wildcard src/*/*/*/*.cc) $(wildcard src/*/*/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=./build/Cobra

all: 
	make buildAll
	make lib

cb: 
	./configure.py -f

buildAll: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)

lib:
	@ar cr build/libcobra.a $(wildcard src/*/*/*/*.o) $(wildcard src/*/*/*.o) $(wildcard src/*/*.o) $(wildcard src/*.o)

.cc.o: $(OBJS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete
	rm -rf build/*

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra ${ARGS}

shell:
	g++ $(FLAGS) -I ./ test/shell.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra ${ARGS}

mem:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./build/Cobra

mt:
	make
	make test

cmt:
	make clean
	make
	make test

docs:
	doxygen documentation/doxygen.config

open:
	open documentation/doxygen/html/index.html

.PHONY: all clean test