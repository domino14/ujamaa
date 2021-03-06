#ifndef _GADDAG_H_
#define _GADDAG_H_


// bugs in Gordon's algorithm:
// - addfinalarc should also update state (doesn't explicitly say)
// - letters should be stored in state, not arc. else we have a problem:
// some arcs pointing to the same node do not get their letterSets updated.
// (work out simple CARE example w/algorithm)


#define SEPARATION_TOKEN '^'
#define TRUE 1
#define FALSE 0
#define NUM_LETTERS 26
#define DEBUG FALSE
#define MAX_NODES 3000000
#define MAX_ARCS 4000000
#include <stdint.h>


struct Node {
    struct Arc **arcs;
    /**
     * Number of arcs leaving this node.
     */
    uint8_t numArcs;
    /*
     * arcBitVector is a bit vector of letters corresponding to each Arc.
     */
    uint32_t arcBitVector;
    /*
     * letterSet is a bit vector where 0 is A and 25 is Z. It is the set
     * of letters which if encountered next, make a word.
     */
    uint32_t letterSet;
    uint32_t serializedIndex;
};

struct Arc {
    char letter;
    struct Node *destination;
    struct Node *source;
};

typedef struct Node NODE;
typedef struct Arc ARC;

void gen_gaddag(char* filename);
NODE* load_gaddag(char* filename);
char containsLetter(NODE*, char);
#endif