CC=gcc
CFLAGS=-c -Wall
SOURCES=src/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ujamaa


all: $(SOURCES) $(EXECUTABLE) makegaddag

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf src/*.o $(EXECUTABLE) makegaddag

makegaddag: src/makegaddag/gaddag.o
	$(CC) src/makegaddag/gaddag.o -o $@
