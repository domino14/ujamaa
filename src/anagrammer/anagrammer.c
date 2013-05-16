#include <string.h>
#include "../movegen.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

extern struct GameState game_state;

void anagram(NODE* node, char* str) {
    ARC* arc = malloc(sizeof(ARC));
    game_state.num_distinct_letters = 27;
    /* some dummy values needed for game state */
    game_state.current_anchor_row = 0;
    game_state.current_anchor_col = 0;
    game_state.game_board[0][0] = '.';
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
    gen(0, init_word, rack, arc, 0);

    free(arc);
}

/*
 * Finds all anagrams for the string str.
 * Blanks can be represented by '?'.
 */

// CARE:
// C?ARE AC?RE RAC?E ERAC
// void anagram(NODE* node, char* str) {

// }

// void findSubwords(NODE* node) {

// }

/*
 * If any of the rotations of the word is in the gaddag, then the word
 * is in the gaddag. But we are not cool enough to use parallelism yet
 * so just find the flipped word.
 * @param {NODE*} node The initial node.
 * @param {char*} str The string to find.
 * @return {NODE*} node The node of the final letter or NULL if not found.
 */
// int findPartialWord(NODE* node, char* str) {
//     int len = strlen(str);
//     int i, j;
//     int letterFound;
//     NODE* curNode = node;
//     for (i = len - 1; i >= 0; i--) {
//         letterFound = FALSE;
//         for (j = 0; j < curNode->numArcs; j++) {
//             if (curNode->arcs[j]->letter == str[i]) {
//                 curNode = curNode->arcs[j]->destination;
//                 letterFound = TRUE;
//                 break;  // Found letter for node
//             }
//         }
//         if (!letterFound) {
//             return NULL;
//         }
//     }
//     return curNode;
// }
