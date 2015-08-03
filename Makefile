CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES= $(wildcard src/cobra/*/*.cc)

OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=./build/Cobra

all: 
	make buildAll
	make lib

buildAll: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -c

lib:
	ar rvs build/libcobra.a $(wildcard src/cobra/*/*.o)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(wildcard src/cobra/*/*.o)
	rm -rf build/*.a

test:
	g++ $(FLAGS) -I ./ test/main.cc -Iinclude build/libcobra.a -o build/Cobra 
	./build/Cobra ${ARGS}

.PHONY: all clean test