CC=g++
CFLAGS=-std=c++11 -c -I src/ -g -MD -MP
LDFLAGS=
SOURCES=$(wildcard src/*/*/*/*.cc) $(wildcard src/*/*/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
PLATFORM=$(shell uname)
EXECUTABLE=./build/$(PLATFORM)/Condor

ifeq ($(shell uname), Linux)
  PLATFORM=linux
endif
ifeq ($(shell uname), linux)
  PLATFORM=linux
endif
ifeq ($(shell uname), Darwin)
  PLATFORM=darwin
endif
ifeq ($(shell uname), CYGWIN_NT-4.0)
  PLATFORM=win32
endif
ifeq ($(shell uname), CYGWIN_NT-5.0)
  PLATFORM=win32
endif
ifeq ($(shell uname), CYGWIN_NT-5.1)
  PLATFORM=win32
endif
ifeq ($(shell uname), MINGW32_NT-4.0)
  PLATFORM=win32
endif
ifeq ($(shell uname), MINGW32-5.0)
  PLATFORM=win32
endif
ifeq ($(shell uname), MINGW32_NT-5.1)
  PLATFORM=win32
endif
ifeq ($(shell uname -m), IP19)
  PLATFORM=mips3
endif
ifeq ($(shell uname -m), IP22)
  PLATFORM=mips3
endif
ifeq ($(shell uname -m), IP25)
  PLATFORM=mips4
endif
ifeq ($(shell uname), IRIX64)
  PLATFORM=mips4
endif

mt:
	make all
	make test

all: 
	make buildAll
	make lib

sem: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcondor.a -o build/$(PLATFORM)/Condor
	./build/$(PLATFORM)/Condor --trace-semantic

parser: 
	make all
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcondor.a -o build/$(PLATFORM)/Condor
	./build/$(PLATFORM)/Condor --trace-parser

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
	rm -rf build/*/*

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcondor.a -o build/$(PLATFORM)/Condor
	make t

t:
	./build/$(PLATFORM)/Condor ${ARGS} test/test.cb

shell:
	g++ $(FLAGS) -I ./ test/shell.cc -Iinclude build/libcondor.a -o build/$(PLATFORM)/Condor
	./build/$(PLATFORM)/Condor ${ARGS}

mem:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --dsymutil=yes ./build/$(PLATFORM)/Condor > log.txt 2>&1

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
	gdb ./build/$(PLATFORM)/Condor

docs:
	doxygen documentation/doxygen.config

open:
	open documentation/doxygen/html/index.html

.PHONY: all clean test