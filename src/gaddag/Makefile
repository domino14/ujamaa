CC=gcc
CFLAGS=-c -Wall
SOURCES=gaddag.c makegaddag.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=makegaddag


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

