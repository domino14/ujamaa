#ifndef _ANAGRAMMER_H_
#define _ANAGRAMMER_H_

#include "../gaddag/gaddag.h"
#include <glib.h>

void anagram(NODE* node, char* str, char* mode);
void anagram_gen(int pos, char* word, uint8_t* rack, ARC* arc, uint8_t mode);
void anagram_go_on(int pos, char L, char* word, uint8_t* rack, ARC* NewArc,
                   ARC* OldArc, uint8_t mode);
void add_play(char*);
void answer_print_fn(char* key, char* value, gpointer userdata);
#endif