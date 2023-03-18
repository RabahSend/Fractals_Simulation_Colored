CC = g++
CFLAGS = -Wall -std=c++17 -Iinclude
LDLIBS = -lsfml-graphics -lsfml-window -lsfml-system
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=obj/%.o)
TARGET = fractal

all: $(TARGET)

obj:
	mkdir -p obj

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

obj/%.o: src/%.cpp | obj
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ) $(TARGET)
