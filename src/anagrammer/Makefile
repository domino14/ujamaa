CC=gcc
CFLAGS=-c -Wall $(shell pkg-config --cflags glib-2.0) -O3 \
	$(shell pkg-config --cflags gio-2.0)
SOURCES=../gaddag/gaddag.c main.c anagrammer.c ../movegen.c ../server.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=anagrammer

# Blank challenges
SOURCES_CHALLENGES=gen_blank_challenges.c ../movegen.c ../gaddag/gaddag.c \
	anagrammer.c
OBJECTS_CHALLENGES=$(SOURCES_CHALLENGES:.c=.o)
EXECUTABLE_CHALLENGES=blank_challenges

# Judge
SOURCES_JUDGE=../gaddag/gaddag.c judge.c
OBJECTS_JUDGE=$(SOURCES_JUDGE:.c=.o)
EXECUTABLE_JUDGE=judge

LIBS=$(shell pkg-config --libs glib-2.0)
SOCKET_LIBS=$(shell pkg-config --libs gio-2.0)

all: $(SOURCES) $(EXECUTABLE) $(SOURCES_CHALLENGES) $(EXECUTABLE_CHALLENGES) \
     $(SOURCES_JUDGE) $(EXECUTABLE_JUDGE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS) $(SOCKET_LIBS)

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE_CHALLENGES): $(OBJECTS_CHALLENGES)
	$(CC) $(OBJECTS_CHALLENGES) -o $@ $(LIBS)

$(EXECUTABLE_JUDGE): $(OBJECTS_JUDGE)
	$(CC) $(OBJECTS_JUDGE) -o $@

clean:
	rm -rf *.o $(EXECUTABLE) $(EXECUTABLE_CHALLENGES) ../gaddag/*.o ../*.o \
    $(EXECUTABLE_JUDGE)
