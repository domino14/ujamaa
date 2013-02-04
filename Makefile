CC=gcc
CFLAGS=-c -Wall
SOURCES=src/main.c src/board.c src/movegen.c src/gaddag/gaddag.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ujamaa


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf src/*.o $(EXECUTABLE)
