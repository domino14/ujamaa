#include <string.h>
#include "../movegen.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/time.h>
#include "anagrammer.h"

extern struct GameState game_state;
GHashTable *answers_hash;

#define ANAGRAM_MODE 0
#define BUILD_MODE 1
/**
 * Adapted version of Gordon's "gen" function.
 * @param pos  [description]
 * @param word [description]
 * @param rack [description]
 * @param arc  [description]
 * @param mode 0 anagram, 1 build
 */
void anagram_gen(int pos, char* word, uint8_t* rack, ARC* arc, uint8_t mode) {
    uint8_t blank_position = game_state.num_distinct_letters - 1;
    uint8_t i, k;
    if (letters_remain(rack)) {
        /*
         * TODO: for each letter ALLOWED ON THIS SQUARE, not just all
         * letters.
         */
        // For all letters, except the blank.
        for (i = 0; i < game_state.num_distinct_letters - 1; i++) {
            if (rack[i] > 0) {
                // Letter (i + 'A') is on this rack. Temporarily remove it.
                rack[i]--;
                anagram_go_on(pos, i + 'A', word, rack, next_arc(arc, i + 'A'),
                      arc, mode);
                // Re-add letter.
                rack[i]++;
            }
        }
        // Check if there is a blank.
        if (rack[blank_position] > 0) {
            // For each blank
            for (k = 0; k < game_state.num_distinct_letters; k++) {
                /**
                 * TODO: For each letter the blank could be ALLOWED
                 * ON THIS SQUARE.
                 */
                rack[blank_position]--;
                anagram_go_on(pos, k + 'A', word, rack, next_arc(arc, k + 'A'),
                      arc, mode);
                rack[blank_position]++;
            }
        }
    }
}

void anagram_go_on(int pos, char L, char* word, uint8_t* rack, ARC* NewArc,
    ARC* OldArc, uint8_t mode) {
    char word_c[16];
    char word_copy[16];
    word_c[0] = L;
    word_c[1] = '\0';
    if (pos <= 0) {
        // word <- L || word
        strcat(word_c, word);
        // word_c now prepends word with L. Can I do this faster?
        // IF L on oldArc & no letter directly left then RecordPlay
        if (containsLetter(OldArc->destination, L)) {
            if (mode == BUILD_MODE ||
                    (mode == ANAGRAM_MODE && !letters_remain(rack))) {
                add_play(word_c);
            }
        }
        if (NewArc) {
            anagram_gen(pos - 1, word_c, rack, NewArc, mode);
            NewArc = next_arc(NewArc, SEPARATION_TOKEN);
            // Now shift direction.
            if (NewArc) {
                anagram_gen(1, word_c, rack, NewArc, mode);
            }
        }
    } else if (pos > 0) {
        strcpy(word_copy, word);
        strcat(word_copy, word_c);
        if (containsLetter(OldArc->destination, L)) {
            if (mode == BUILD_MODE ||
                    (mode == ANAGRAM_MODE && !letters_remain(rack))) {
                add_play(word_copy);
            }
        }
        if (NewArc) {
            // TODO check if room to the right.
            anagram_gen(pos + 1, word_copy, rack, NewArc, mode);
        }
    }
}

void anagram(NODE* node, char* str, char* mode) {
    struct timeval start_time, end_time;
    int total_usecs;
    gettimeofday(&start_time, NULL);
    ARC* arc = malloc(sizeof(ARC));
    game_state.num_distinct_letters = 27;
    char init_word[16];
    uint8_t rack[27];
    uint8_t i;
    strcpy(init_word, "");
    // Set the arc to point to the initial node.
    arc->destination = node;
    for (i = 0; i < 27; i++) {
        rack[i] = 0;
    }
    // Turn the passed in string into a rack.
    for (i = 0; i < strlen(str); i++) {
        if (str[i] != '?') {
            rack[toupper(str[i]) - 'A']++;
        } else {
            rack[26]++;
        }
    }
    answers_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
                                         NULL);

    if (strcmp(mode, "anagram") == 0) {
        anagram_gen(0, init_word, rack, arc, 0);
    } else if (strcmp(mode, "build") == 0) {
        anagram_gen(0, init_word, rack, arc, 1);
    }
    // Print out hash contents
    g_hash_table_foreach(answers_hash, answer_print_fn, NULL);
    printf("\n");
    printf("Found %d words\n", g_hash_table_size(answers_hash));

    gettimeofday(&end_time, NULL);
    total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
        (end_time.tv_usec-start_time.tv_usec);
    printf("Took %d usecs\n", total_usecs);
    free(arc);
}

void add_play(char* word) {
    char* word_p = malloc(strlen(word));
    strcpy(word_p, word);
    g_hash_table_add(answers_hash, word_p);
}

void answer_print_fn(char* key, char* value, gpointer userdata) {
    printf("%s ", key);
}