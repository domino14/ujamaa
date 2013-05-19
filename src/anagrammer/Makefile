CC=gcc
CFLAGS=-c -Wall $(shell pkg-config --cflags glib-2.0) -O3
SOURCES=../gaddag/gaddag.c main.c anagrammer.c ../movegen.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=anagrammer

# Blank challenges
SOURCES_CHALLENGES=gen_blank_challenges.c ../movegen.c ../gaddag/gaddag.c anagrammer.c
OBJECTS_CHALLENGES=$(SOURCES_CHALLENGES:.c=.o)
EXECUTABLE_CHALLENGES=blank_challenges

LIBS=$(shell pkg-config --libs glib-2.0)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE_CHALLENGES): $(OBJECTS_CHALLENGES)
	$(CC) $(OBJECTS_CHALLENGES) -o $@ $(LIBS)

clean:
	rm -rf *.o $(EXECUTABLE) $(EXECUTABLE_CHALLENGES) ../gaddag/*.o ../*.o

$(EXECUTABLE_CHALLENGES): $(OBJECTS_CHALLENGES)