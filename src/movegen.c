#include "movegen.h"
#include <stdio.h>
#include <string.h>

//#define DEBUG_
struct GameState game_state;

void gen_moves(char** position) {
    // generates all valid moves given a position array
    // position array is basically a representation of the board (
    // usually 15x15)
    // squares:
    // 3 - 3W score
    // 2 - 2W score
    // @ - 2L score
    // # - 3L score
    // . - blank square, no bonuses
    // any capital letter - that tile
    // any lowercase letter - a blank
}

/**
 * Returns letter if this is a letter, 0 otherwise.
 * @param  letter A maybe-letter.
 * @return        See above.
 */
uint8_t is_letter(char letter) {
    if ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z')) {
        return letter;
    }
    return 0;
}

void print_rack(uint8_t* rack) {
    uint8_t i, j;
    printf(" Rack: ");
    for (i = 0; i < game_state.num_distinct_letters; i++) {
        for (j = 0; j < rack[i]; j++) {
            if (i != game_state.num_distinct_letters - 1) {
                printf("%c", i + 'A');
            } else {
                printf("?");
            }
        }
    }
    printf("\n");
}

void print_spaces(uint8_t num_spaces) {
    uint8_t i;
    for (i = 0; i < num_spaces; i++) {
        printf("-");
    }
}

/**
 * From Gordon's paper, the gen function.
 * @param pos  The offset from an anchor square.
 * @param word The word generated so far.
 * @param rack The player's current rack.
 * @param arc  An Arc to the initial state of the GADDAG.
 */
void gen(int pos, char* word, uint8_t* rack, ARC* arc, int spaces) {
    // If a letter L is on this square, go_on
    // What square am I on?
    #ifdef DEBUG_
    print_spaces(spaces);
    printf("in gen, %d, Word: %s,", pos, word);
    print_rack(rack);
    #endif
    uint8_t blank_position = game_state.num_distinct_letters - 1;
    uint8_t i, k;
    char letter = is_letter(
        game_state.game_board[game_state.current_anchor_row]
        [game_state.current_anchor_col]);
    if (letter) {
        go_on(pos, letter, word, rack, next_arc(arc, letter), arc, spaces);
    } else if (letters_remain(rack)) {
        /*
         * TODO: for each letter ALLOWED ON THIS SQUARE, not just all
         * letters.
         */
        // For all letters, except the blank.
        for (i = 0; i < game_state.num_distinct_letters - 1; i++) {
            if (rack[i] > 0) {
                #ifdef DEBUG_
                print_spaces(spaces);
                printf("%d %cs\n", rack[i], i + 'A');
                #endif
                // Letter (i + 'A') is on this rack. Temporarily remove it.
                rack[i]--;
                go_on(pos, i + 'A', word, rack, next_arc(arc, i + 'A'),
                      arc, spaces + 1);
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
                go_on(pos, k + 'A', word, rack, next_arc(arc, k + 'A'),
                      arc, spaces + 1);
                rack[blank_position]++;
            }
        }
    }
}

/**
 * The goOn function from Gordon's paper.
 * @param pos    The position relative to the current anchor square.
 * @param L      A letter from 'A' to 'Z'.
 * @param word   A possible word being built.
 * @param rack   An integer describing the rack.
 * @param NewArc The next arc based on the L parameter.
 * @param OldArc The old arc.
 */
void go_on(int pos, char L, char* word, uint8_t* rack, ARC* NewArc,
    ARC* OldArc, int spaces) {
    #ifdef DEBUG_
    print_spaces(spaces);
    printf("in go_on, %d, Letter: %c, Word: %s,", pos, L, word);
    print_rack(rack);
    #endif
    char word_c[16];
    char word_copy[16];
    word_c[0] = L;
    word_c[1] = '\0';
    if (pos <= 0) {
        // word <- L || word
        #ifdef DEBUG_
        print_spaces(spaces);
        printf("pos <= 0, here\n");
        #endif
        strcat(word_c, word);
        // word_c now prepends word with L. Can I do this faster?
        // IF L on oldArc & no letter directly left then RecordPlay
        if (containsLetter(OldArc->destination, L)) {
            // TODO - check if no letter directly left, this is also a
            // required condition.
            #ifdef DEBUG_
            print_spaces(spaces);
            printf("containsLetter(OldArc->destination, %c)\n", L);
            #endif
            record_play(word_c);
        }
        if (NewArc) {
            #ifdef DEBUG_
            print_spaces(spaces);
            printf("New arc\n");
            #endif
            // If there is room to the left, Gen(pos-1, word, rack, NewArc)
            // TODO, check if there is room to the left.
            if (1) {
                gen(pos - 1, word_c, rack, NewArc, spaces + 1);
            }
            NewArc = next_arc(NewArc, SEPARATION_TOKEN);
            // Now shift direction.
            if (NewArc) {
                #ifdef DEBUG_
                print_spaces(spaces);
                printf("Switched direction\n");
                #endif
                // TODO check if there is no letter directly left, AND
                // there is room to the right.
                if (1) {
                    gen(1, word_c, rack, NewArc, spaces + 1);
                }
            }
        }
    } else if (pos > 0) {
        #ifdef DEBUG_
        print_spaces(spaces);
        printf("pos > 0\n");
        #endif
        strcpy(word_copy, word);
        strcat(word_copy, word_c);
        if (containsLetter(OldArc->destination, L)) {
            #ifdef DEBUG_
            print_spaces(spaces);
            printf("containsLetter(OldArc->destination, %c)\n", L);
            #endif
            // TODO check if no letter directly right.
            if (1) {
                record_play(word_copy);
            }
        }
        if (NewArc) {
            #ifdef DEBUG_
            print_spaces(spaces);
            printf("NewArc\n");
            #endif
            // TODO check if room to the right.
            if (1) {
                gen(pos + 1, word_copy, rack, NewArc, spaces + 1);
            }
        }
    }
}

/**
 * Gets the next arc, given an initial arc and a letter.
 * @param  arc    Initial arc.
 * @param  letter Letter.
 * @return        An ARC.
 */
ARC* next_arc(ARC* arc, char letter) {
    NODE* node = arc->destination;
    uint8_t i;
    for (i = 0; i < node->numArcs; i++) {
        if (node->arcs[i]->letter == letter) {
            return node->arcs[i];
        }
    }
    return NULL;
}

/**
 * Returns 1 if there is at least one letter in the rack, 0 otherwise.
 * @param  rack An array of 27 integers, the letter count of each letter.
 *              0 = A, Z = 25, ? = 26
 * @return      See above.
 */
uint8_t letters_remain(uint8_t* rack) {
    uint8_t i;
    for (i = 0; i < game_state.num_distinct_letters; i++) {
        if (rack[i] > 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Records a play.
 * @param word A word.
 */
void record_play(char* word) {
    // TODO for right now this just prints out the word.
    printf("%s\n", word);
}