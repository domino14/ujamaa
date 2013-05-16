#ifndef _MOVEGEN_H_
#define _MOVEGEN_H_

#include "board.h"
#include "gaddag/gaddag.h"
#include <stdint.h>

struct GameState {
    /*
     * A 3-d array of cross_checks, stored as a bit vector for each square
     * of the board.
     * These are "horizontal" and "vertical" cross-checks for each square.
     */
    uint32_t cross_checks[15][15][2];
    /*
     * Anchor squares; 1 for anchor, 0 for no anchor.
     */
    uint8_t anchors[15][15];
    /*
     * The current anchor's row or col.
     */
    uint8_t current_anchor_row;
    uint8_t current_anchor_col;
    /*
     * The game board.
     */
    char game_board[15][15];

    uint8_t num_distinct_letters;
};

void gen_moves(char** position);
void gen(int pos, char* word, uint8_t* rack, ARC* arc, int);
void go_on(int pos, char L, char* word, uint8_t* rack, ARC* NewArc,
    ARC* OldArc, int);
ARC* next_arc(ARC* arc, char letter);
uint8_t letters_remain(uint8_t* rack);
void record_play(char* word);

#endif