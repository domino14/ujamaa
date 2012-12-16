#ifndef _GADDAG_H_
#define _GADDAG_H_

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
    uint8_t numArcs;
    // letterSet is a bit vector where 0 is A and 25 is Z
    uint32_t arcBitVector;
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
#endif