#include "gaddag.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdint.h>

#define SEPARATION_TOKEN '^'
#define TRUE 1
#define FALSE 0
#define NUM_LETTERS 26
#define DEBUG FALSE
#define MAX_NODES 3000000
#define MAX_ARCS 4000000


struct Node {
    struct Arc **arcs;
    uint8_t numArcs;
    // letterSet is a bit vector where 0 is A and 25 is Z
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

NODE* nodeArr[MAX_NODES];
ARC* arcArr[MAX_ARCS];

NODE *initialState;

uint32_t allocArcs = 0, allocStates = 0;

uint32_t findNodeInArray(NODE* node) {
    uint32_t i;
    for (i = 0; i < allocStates; i++) {
        if (nodeArr[i] == node) {
            return i;
        }
    }
    assert(0);
}

char containsLetter(NODE* node, char c) {
    char letter = toupper(c);
    int bit = 1 << (letter - 'A');
    if (node->letterSet & bit) {
        return TRUE;
    }
    return FALSE;
}

ARC* containsArc(NODE* node, char c) {
    int i, index;
    if (DEBUG) {
        index = findNodeInArray(node);
        printf("  Calling containsArc on node %d, %c, numArcs %d\n",
               index, c, node->numArcs);
    }
    for (i = 0; i < node->numArcs; i++) {
        if (DEBUG) {
            printf("  Checking arc %p, its letter: %c\n", node->arcs[i],
                   node->arcs[i]->letter);
        }
        if (node->arcs[i]->letter == c) {
            return node->arcs[i];
        }
    }
    return NULL;
}


NODE* createNode() {
    // creates and initializes a node
    NODE* newNode = malloc(sizeof(NODE));
    newNode->letterSet = 0;
    newNode->arcs = NULL;
    newNode->numArcs = 0;
    nodeArr[allocStates] = newNode;
    allocStates++;
    return newNode;
}


NODE* createArc(NODE* from, char c, NODE* to) {
    // creates an arc from node named "from", and returns the new node
    // that this arc points to (or if to is not NULL, it returns that)
    int index;
    NODE* newNode;
    ARC* newArc;
    // create the new NODE and ARC
    if (!to) {
        newNode = createNode();
    } else {
        newNode = to;
    }
    newArc = malloc(sizeof(ARC));
    arcArr[allocArcs] = newArc;
    allocArcs++;

    if (!(from->arcs)) {
        // this node's array of arcs is NULL. create that first
        from->arcs = malloc(sizeof(ARC*));
        index = 0;
    } else {
        // reallocate node's array of arcs
        index = from->numArcs;
        if (DEBUG) {
            printf("Reallocating %d number of arcs\n", from->numArcs + 1);
        }
        from->arcs = realloc(from->arcs, (from->numArcs + 1) * sizeof(ARC*));
    }
    from->arcs[index] = newArc;
    from->numArcs++;
    newArc->destination = newNode;
    newArc->letter = c;
    newArc->source = from;
    return newNode;
}

NODE* AddArc(NODE* state, char c) {
    // adds an arc from state for c (if one does not already exist)
    // and resets state to the node this arc leads to.
    // every state has an array of Arc pointers. we need to create the
    // Array if it doesn't exist.
    // returns the created or existing NODE
    int index;
    if (DEBUG) {
        index = findNodeInArray(state);
        printf("Called AddArc for state %d, char %c\n", index, c);
    }

    ARC* existingArc;
    NODE* nextNode;
    existingArc = containsArc(state, c);

    if (!existingArc) {
        // it's NULL, create it
        nextNode = createArc(state, c, NULL);
    } else {
        nextNode = existingArc->destination;
    }

    if (DEBUG) {
        index = findNodeInArray(nextNode);
        if (existingArc) {
            printf("Found existing node %d\n", index);
        }
        else {
            printf("Created new node %d\n", index);
        }
        printf("Updating state to %d\n", index);
    }
    return nextNode;
}

NODE* AddFinalArc(NODE *state, char c1, char c2) {
    // add arc from state to c1 and add c2 to this arc's letter set
    NODE* nextNode;
    int index;
    int bit;
    if (DEBUG) {
        index = findNodeInArray(state);
        printf("->(Called AddFinalArc for state %d, chars %c, %c)\n",
               index, c1, c2);
    }
    nextNode = AddArc(state, c1);
    assert(!containsLetter(nextNode, c2));
    if (DEBUG) {
        index = findNodeInArray(nextNode);
        printf("->(Setting %c in %d's letterSet)\n", c2, index);
    }
    bit = 1 << (toupper(c2) - 'A');
    nextNode->letterSet |= bit;
    return nextNode;
}

void ForceArc(NODE *state, char c, NODE *forceState) {
    // add an arc from state to forceState for c (an error occurs if
    // an arc from st for c already exists going to any other state)
    ARC* arc;
    int index1, index2;
    if (DEBUG) {
        index1 = findNodeInArray(state);
        index2 = findNodeInArray(forceState);
        printf("Forcing Arc for state %d to state %d, char %c\n",
               index1, index2,c);
    }
    arc = containsArc(state, c);
    if (arc && arc->destination != forceState) {
        index1 = findNodeInArray(arc->destination);
        printf("Arc already exists; destination %d %c\n",
               index1, arc->letter);
        assert(0);
    }
    assert(createArc(state, c, forceState) == forceState);
}

int get_words(char* filename, char words[][16]) {
    FILE *fp;
    char buffer[500];
    int numWords = 0;
    int pos = 0;
    fp = fopen(filename, "rb");
    if (fp == 0) {
        printf("Could not load file.\n");
        return 0;
    }
    while (fgets(buffer, 250, fp)) {
        // find first occurrence of space
        pos = strcspn(buffer, " ");
        // only copy words until the space
        strncpy(words[numWords], buffer, pos);
        numWords++;
    }
    fclose(fp);
    printf("Read %d words from file\n", numWords);
    return numWords;
}

// char* append_char(char* word, char c) {
//     // appends char c to the end of word
//     int length = strlen(word);
//     word[length] = c;
//     word[length + 1] = '\0';
//     return word;
// }

// void depth_first_search(NODE* root) {
//     int i, j;
//     char letter;
//     char word[17] = "";  // 15 max length, ^ and \0
//     char* letterSet;
//     for (i = 0; i < root->numArcs; i++) {
//         word = append_char(word, root->arcs[i]->letter);
//         letterSet = root->arcs[i]->destination->letterSet;
//         for (j = 0; j < NUM_LETTERS; j++) {
//             if (letterSet[j]) {
//                 letter = 'A' + j;
//                 word = append_char(word, letter);
//             }
//         }
//     }
// }

// void test_gaddag(char words[][16], int numWords, NODE* root) {
// }

void serialize_node(uint32_t *bitVector, uint32_t *letterSet, NODE* node) {
    // for each node, use two 32-bit words; one is a bit vector indicating
    // a letter for each arc that follows it, the other is the letter
    // set
    int j;
    (*bitVector) = 0;
    for (j = 0; j < node->numArcs; j++) {
        char letter = toupper(node->arcs[j]->letter);
        if (letter != SEPARATION_TOKEN) {
            (*bitVector) += (1 << (letter - 'A'));
        }
        else {
            (*bitVector) += (1 << 26);
        }
    }
    (*letterSet) = node->letterSet;
}

int compare_arcs (const ARC *a, const ARC *b)
{
  int temp = *a->letter - *b->letter;
  if (temp > 0)
    return 1;
  else if (temp < 0)
    return -1;
  else
    return 0;
}


void save_gaddag(char* filename) {
    // i've had enough of trying to serialize this efficiently
    // this is an ugly way that works. whatever.
    // would be significantly sped up if "findNodeInArray"
    // were to use hash tables.
    NODE* node;
    ARC* arc;
    void** serialized;
    int num_elements = allocStates * 2 + allocArcs;
    int i, j;
    serialized = malloc(sizeof(uint32_t*) * num_elements);
    // note, this assumes a pointer's size is at least as big as a uint32_t
    // this should be true for pretty much every modern computer out there...
    int32_t bitVector, letterSet;
    int idx = 0;
    for (i = 0; i < allocStates; i++) {
        node = nodeArr[i];
        serialize_node(&bitVector, &letterSet, node);
        serialized[idx] = bitVector;
        node->serializedIndex = idx;
        idx++;
        serialized[idx] = letterSet;
        idx++;
        // search for arc in letter order
        // qsort is defined in stdlib.h
        qsort(node->arcs, node->numArcs, sizeof(ARC*), compare_arcs);
        // store the negative address of each node's "serializedIndex"
        // so that we can dereference it later on the second pass-through.
        for (j = 0; j < node->numArcs; j++) {
            serialized[idx] = -&(node->arcs[j]->destination->serializedIndex);
            idx++;
        }
    }

    FILE* fp = fopen(filename, "wb");
    for (i = 0; i < num_elements; i++) {
        if (serialized[i] >= 0) {
            fwrite(&(serialized[i]), sizeof(int32_t), 1, fp);
        }
        else {
            // this is a pointer, dereference the negative and write to file
            fwrite(&(*(-serialized[i])), sizeof(int32_t), 1, fp);
        }
    }



    fclose(fp);
}


void gen_gaddag(char* filename) {
    char words[300000][16];
    int numWords;
    int i = 0, j = 0, n = 0, m;
    NODE* st;
    NODE* forceSt;
    numWords = get_words(filename, words);

    if (DEBUG) {
        numWords = 2;
        strcpy(words[0], "CAREEN");
        strcpy(words[1], "CARREL");
    }

    initialState = createNode();
    for (i = 0; i < numWords; i++) {
        if (i % 10000 == 0) {
            printf("%d...\n", i);
        }
        st = initialState;
        if (DEBUG) {
            printf("%s\n", words[i]);
        }
        // create path for anan-1...a1:
        n = strlen(words[i]);
        for (j = n - 1; j >= 2; j--) {
            st = AddArc(st, words[i][j]);
        }
        st = AddFinalArc(st, words[i][1], words[i][0]);

        // create path for an-1...a1^an
        st = initialState;
        for (j = n - 2; j >= 0; j--) {
            st = AddArc(st, words[i][j]);
        }
        st = AddFinalArc(st, SEPARATION_TOKEN, words[i][n - 1]);

        // partially minimize remaining paths
        for (m = n - 3; m >= 0; m--) {
            forceSt = st;
            st = initialState;
            for (j = m; j >= 0; j--) {
                st = AddArc(st, words[i][j]);
            }
            st = AddArc(st, SEPARATION_TOKEN);
            ForceArc(st, words[i][m + 1], forceSt);
        }
    }
    printf("Allocated arcs: %d states: %d\n", allocArcs, allocStates);
    save_gaddag("out.gaddag");
}

// bugs in Gordon's algorithm:
// - addfinalarc should also update state (doesn't explicitly say)
// - letters should be stored in state, not arc. else we have a problem:
// some arcs pointing to the same node do not get their letterSets updated.
// (work out simple CARE example w/algorithm)

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s [argument]\n", argv[0]);
    } else {
        gen_gaddag(argv[1]);
    }
    return 0;
}
