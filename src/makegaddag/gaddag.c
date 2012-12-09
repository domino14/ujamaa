#include "gaddag.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define SEPARATION_TOKEN '$'
#define TRUE 1
#define FALSE 0
#define NUM_LETTERS 26
#define DEBUG FALSE
#define MAX_NODES 3000000
#define MAX_ARCS 4000000


struct Node {
    struct Arc **arcs;
    int numArcs;
    // letterSet is an array of booleans for that letter in the alphabet,
    // where 0 is A and 25 is Z
    char letterSet[NUM_LETTERS];
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

int findNodeInArray(NODE* node) {
    int i;
    for (i = 0; i < MAX_NODES; i++) {
        if (nodeArr[i] == node) {
            return i;
        }
    }
    // did not find node!
    assert(0);
}

char containsLetter(NODE* node, char c) {
    char letter = toupper(c);
    if (node->letterSet[letter - 'A']) {
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

NODE *initialState;

int allocArcs = 0, allocStates = 0;

NODE* createNode() {
    // creates and initializes a node
    NODE* newNode = malloc(sizeof(NODE));
    int i;
    for (i = 0; i < NUM_LETTERS; i++) {
        newNode->letterSet[i] = FALSE;
    }
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
    nextNode->letterSet[toupper(c2) - 'A'] = TRUE;
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

char* append_char(char* word, char c) {
    // appends char c to the end of word
    int length = strlen(word);
    word[length] = c;
    word[length + 1] = '\0';
    return word;
}

void depth_first_search(NODE* root) {
    int i, j;
    char letter;
    char word[17] = "";  // 15 max length, $ and \0
    char* letterSet;
    for (i = 0; i < root->numArcs; i++) {
        word = append_char(word, root->arcs[i]->letter);
        letterSet = root->arcs[i]->destination->letterSet;
        for (j = 0; j < NUM_LETTERS; j++) {
            if (letterSet[j]) {
                letter = 'A' + j;
                word = append_char(word, letter);
            }
        }
    }
}

void test_gaddag(char words[][16], int numWords, NODE* root) {
}

void gen_gaddag(char* filename) {
    char words[300000][16];
    int numWords;
    int i = 0, j = 0, n = 0, m;
    NODE* st;
    NODE* forceSt;
    numWords = get_words(filename, words);
    // numWords = 5;
    // strcpy(words[0], "AA");
    // strcpy(words[1], "AB");
    // strcpy(words[2], "AH");
    // strcpy(words[3], "BA");
    // strcpy(words[4], "AAH");

    // numWords = 1;
    // strcpy(words[0], "CAREEN");
    initialState = createNode();

    for (i = 0; i < numWords; i++) {
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

        // create path for an-1...a1$an
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
    test_gaddag(words, numWords, initialState);
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
