#ifndef _ANAGRAMMER_H_
#define _ANAGRAMMER_H_

#include "../gaddag/loadgaddag.h"
#include <glib.h>

struct Answers {
    int num_answers;
    char** answers;
    int cur_answer;
};

int anagram(char* str, char* mode, struct Answers *answers,
             int timeit, int quit_if_larger_than);
void anagram_gen(int pos, char* word, uint8_t* rack, ARC* arc, uint8_t mode);
void anagram_go_on(int pos, char L, char* word, uint8_t* rack, ARC* NewArc,
                   ARC* OldArc, uint8_t mode);
void add_play(char*);
void answer_add(char* key, char* value, gpointer userdata);
void cleanup_answers(struct Answers* answers);
void print_answers(struct Answers* answers);
void cleanup_after_anagram();
#endif