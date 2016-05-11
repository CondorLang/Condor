CC=g++
CFLAGS=-std=c++11 -c -I src/ -g -MD -MP
LDFLAGS=
SOURCES=$(wildcard src/*/*/*/*.cc) $(wildcard src/*/*/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=./build/Stone

mt:
	make all
	make test

all: 
	make buildAll
	make lib

sem: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libstone.a -o build/Stone
	./build/Stone --trace-semantic

parser: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libstone.a -o build/Stone
	./build/Stone --trace-parser

cb: 
	./configure -f

buildAll: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)

# http://stackoverflow.com/questions/19726649/how-to-recompile-when-modified-c-header
-include $(OBJECTS:.o=.d)

lib:
	@ar cr ./build/libstone.a $(wildcard src/*/*/*/*.o) $(wildcard src/*/*/*.o) $(wildcard src/*/*.o) $(wildcard src/*.o)

.cc.o: $(OBJS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete
	rm -rf build/*

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libstone.a -o build/Stone
	make t

t:
	./build/Stone ${ARGS} test/test.cb

shell:
	g++ $(FLAGS) -I ./ test/shell.cc -Iinclude build/libstone.a -o build/Stone
	./build/Stone ${ARGS}

mem:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --dsymutil=yes ./build/Stone > log.txt 2>&1

cmt:
	make clean
	make cb
	make
	make test

asm:
	# as src/stone/codegen/test.asm -o src/stone/codegen/test.o
	# ld src/stone/codegen/test.o -e main -o src/stone/codegen/test
	# src/stone/codegen/test
	/usr/local/bin/nasm -f macho64 src/stone/codegen/test.asm && ld -macosx_version_min 10.7.0 -e main -lSystem -o src/stone/codegen/test src/stone/codegen/test.o && ./src/stone/codegen/test

d:
	gdb ./build/Stone

docs:
	doxygen documentation/doxygen.config

open:
	open documentation/doxygen/html/index.html

.PHONY: all clean test