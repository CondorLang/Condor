CC=g++
CFLAGS=-std=c++11 -c -I src/ -g -MD -MP
LDFLAGS=
SOURCES=$(wildcard src/*/*/*/*.cc) $(wildcard src/*/*/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=./build/Cobra

mt:
	make all
	make test

all: 
	make buildAll
	make lib

sem: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra --trace-semantic

parser: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra --trace-parser

cb: 
	./configure -f

buildAll: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)

# http://stackoverflow.com/questions/19726649/how-to-recompile-when-modified-c-header
-include $(OBJECTS:.o=.d)

lib:
	@ar cr ./build/libcobra.a $(wildcard src/*/*/*/*.o) $(wildcard src/*/*/*.o) $(wildcard src/*/*.o) $(wildcard src/*.o)

.cc.o: $(OBJS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete
	rm -rf build/*

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra ${ARGS} test/test.cb

shell:
	g++ $(FLAGS) -I ./ test/shell.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra ${ARGS}

mem:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --dsymutil=yes ./build/Cobra > log.txt 2>&1

cmt:
	make clean
	make cb
	make
	make test

asm:
	# as src/cobra/codegen/test.asm -o src/cobra/codegen/test.o
	# ld src/cobra/codegen/test.o -e main -o src/cobra/codegen/test
	# src/cobra/codegen/test
	/usr/local/bin/nasm -f macho64 src/cobra/codegen/test.asm && ld -macosx_version_min 10.7.0 -e main -lSystem -o src/cobra/codegen/test src/cobra/codegen/test.o && ./src/cobra/codegen/test

d:
	gdb ./build/Cobra

docs:
	doxygen documentation/doxygen.config

open:
	open documentation/doxygen/html/index.html

.PHONY: all clean test