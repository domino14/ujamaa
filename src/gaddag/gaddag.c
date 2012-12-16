#include "gaddag.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

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
    if (arc) {
        if (arc->destination != forceState) {
            index1 = findNodeInArray(arc->destination);
            printf("Arc already exists; destination %d %c\n",
                   index1, arc->letter);
            assert(0);
        } else {
            assert(arc->destination == forceState);
            return;  // don't create the arc if it already exists; redundant.
        }
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

void compute_arc_bitvector(NODE* node) {
    // for a node, compute a bit vector indicating a letter for each arc
    // that follows it
    int j;
    node->arcBitVector = 0;
    for (j = 0; j < node->numArcs; j++) {
        char letter = toupper(node->arcs[j]->letter);
        if (letter != SEPARATION_TOKEN) {
            node->arcBitVector += (1 << (letter - 'A'));
        }
        else {
            node->arcBitVector += (1 << 26);
        }
    }

}

int compare_arcs (const ARC **a, const ARC **b)
{
  int temp = (*a)->letter - (*b)->letter;
  if (temp > 0)
    return 1;
  else if (temp < 0)
    return -1;
  else
    return 0;
}


void save_gaddag(char* filename) {
    NODE* node;
    int num_elements = allocStates * 2 + allocArcs;
    int i, j;
    uint32_t nodeIdx;
    int idx = 0;

    // loop once to compute indices of nodes (node->serializedIndex)
    for (i = 0; i < allocStates; i++) {
        node = nodeArr[i];
        compute_arc_bitvector(node);
        node->serializedIndex = idx;
        idx += 2;   // increment by 2 to make room for both bit vectors
        // search for arc in letter order
        // qsort is defined in stdlib.h
        for (j = 0; j < node->numArcs; j++) {
            idx++;
        }
    }
    assert(idx == num_elements);
    // loop again to write all indices.
    FILE* fp = fopen(filename, "wb");
    idx = 0;
    for (i = 0; i < allocStates; i++) {
        node = nodeArr[i];
        fwrite(&(node->arcBitVector), sizeof(uint32_t), 1, fp);
        fwrite(&(node->letterSet), sizeof(uint32_t), 1, fp);
        qsort(node->arcs, node->numArcs, sizeof(ARC*), compare_arcs);
        for (j = 0; j < node->numArcs; j++) {
            nodeIdx = node->arcs[j]->destination->serializedIndex;
            assert(nodeIdx);
            fwrite(&(nodeIdx), sizeof(uint32_t), 1, fp);
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
