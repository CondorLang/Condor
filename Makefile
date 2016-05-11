CC=g++
CFLAGS=-std=c++11 -c -I src/ -g -MD -MP
LDFLAGS=
SOURCES=$(wildcard src/*/*/*/*.cc) $(wildcard src/*/*/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=./build/Condor

mt:
	make all
	make test

all: 
	make buildAll
	make lib

sem: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcondor.a -o build/Condor
	./build/Condor --trace-semantic

parser: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcondor.a -o build/Condor
	./build/Condor --trace-parser

cb: 
	./configure -f

buildAll: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)

# http://stackoverflow.com/questions/19726649/how-to-recompile-when-modified-c-header
-include $(OBJECTS:.o=.d)

lib:
	@ar cr ./build/libcondor.a $(wildcard src/*/*/*/*.o) $(wildcard src/*/*/*.o) $(wildcard src/*/*.o) $(wildcard src/*.o)

.cc.o: $(OBJS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete
	rm -rf build/*

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcondor.a -o build/Condor
	make t

t:
	./build/Condor ${ARGS} test/test.cb

shell:
	g++ $(FLAGS) -I ./ test/shell.cc -Iinclude build/libcondor.a -o build/Condor
	./build/Condor ${ARGS}

mem:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --dsymutil=yes ./build/Condor > log.txt 2>&1

cmt:
	make clean
	make cb
	make
	make test

asm:
	# as src/condor/codegen/test.asm -o src/condor/codegen/test.o
	# ld src/condor/codegen/test.o -e main -o src/condor/codegen/test
	# src/condor/codegen/test
	/usr/local/bin/nasm -f macho64 src/condor/codegen/test.asm && ld -macosx_version_min 10.7.0 -e main -lSystem -o src/condor/codegen/test src/condor/codegen/test.o && ./src/condor/codegen/test

d:
	gdb ./build/Condor

docs:
	doxygen documentation/doxygen.config

open:
	open documentation/doxygen/html/index.html

.PHONY: all clean test