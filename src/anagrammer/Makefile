CC=gcc
CFLAGS=-c -Wall
SOURCES=../gaddag/gaddag.c main.c anagrammer.c ../movegen.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=anagrammer


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

