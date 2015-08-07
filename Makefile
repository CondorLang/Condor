CC=g++
CFLAGS=-c -I src/
LDFLAGS=
SOURCES=$(wildcard src/*/*/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=./build/Cobra

all: 
	make buildAll
	make lib

buildAll: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -c

lib:
	# ar rvs build/libcobra.a $(wildcard src/cobra/*/*.o)
	@ar cr build/libcobra.a $(wildcard src/*/*/*.o) $(wildcard src/*/*.o) $(wildcard src/*.o)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(wildcard src/cobra/*/*.o)
	rm -rf build/*.a

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra ${ARGS}

tests:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra
	./build/Cobra -tests

mem:
	make
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./build/Cobra

mt:
	make
	make test

.PHONY: all clean test