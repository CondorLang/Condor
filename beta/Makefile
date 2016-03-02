CC := g++
CFLAGS := -c
CCFLAGS := -MMD -MP -Wall -I src/
SRC := $(wildcard src/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*/*/*.cc)

OBJECTS := $(SRC:.cc=.o)
EXECUTABLE := ./build/Cobra

all: $(SRC) $(EXECUTABLE)
$(EXECUTABLE): tool
tool: $(OBJECTS)
	$(CC) -o ./build/Cobra $(OBJECTS)
-include $(OBJECTS:.o=.d)

%.o: %.cc
	$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete
	rm -rf build/*

mt:
	make all
	./build/Cobra

.PHONY: all