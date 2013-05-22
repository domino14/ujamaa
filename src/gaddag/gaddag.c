#include "gaddag.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

NODE* nodeArr[MAX_NODES];

NODE *initialState;

uint32_t bitCount (uint32_t value) {
    uint32_t count = 0;
    while (value > 0) {           // until all bits are zero
        if ((value & 1) == 1)     // check lower bit
            count++;
        value >>= 1;              // shift bits, removing lower bit
    }
    return count;
}

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
    assert(newNode);
    newNode->letterSet = 0;
    newNode->arcs = NULL;
    newNode->numArcs = 0;
    nodeArr[allocStates] = newNode;
    newNode->serializedIndex = allocStates;
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
    assert(newArc);
    allocArcs++;

    if (!(from->arcs)) {
        // this node's array of arcs is NULL. create that first
        from->arcs = malloc(sizeof(ARC*));
        assert(from->arcs);
        index = 0;
    } else {
        // reallocate node's array of arcs
        index = from->numArcs;
        if (DEBUG) {
            printf("Reallocating %d number of arcs\n", from->numArcs + 1);
        }
        from->arcs = realloc(from->arcs, (from->numArcs + 1) * sizeof(ARC*));
        assert(from->arcs);
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
    char buffer[4096];
    int numWords = 0;
    int pos = 0, pos2 = 0;
    int i;
    fp = fopen(filename, "rb");
    if (fp == 0) {
        printf("Could not load file.\n");
        return 0;
    }
    while (fgets(buffer, 4095, fp)) {
        // Find first occurrence of space or \n
        pos = strcspn(buffer, " ");
        pos2 = strcspn(buffer, "\n");
        if (pos2 < pos) {
            pos = pos2;
        }
        // Only copy words until the space or \n
        strncpy(words[numWords], buffer, pos);
        for (i = 0; i < strlen(words[numWords]); i++) {
            assert(words[numWords][i] >='A' && words[numWords][i] <= 'Z');
        }
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
        int toAdd;
        if (letter != SEPARATION_TOKEN) {
            toAdd = 1 << (letter - 'A');
        }
        else {
            toAdd = 1 << 26;
        }
        node->arcBitVector += toAdd;
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

    FILE* fp = fopen(filename, "wb");
    // write the number of elements first
    fwrite(&num_elements, sizeof(uint32_t), 1, fp);
    fwrite(&allocStates, sizeof(uint32_t), 1, fp);

    idx = 0;
    for (i = 0; i < allocStates; i++) {
        node = nodeArr[i];
        compute_arc_bitvector(node);
        fwrite(&(node->arcBitVector), sizeof(uint32_t), 1, fp);
        fwrite(&(node->letterSet), sizeof(uint32_t), 1, fp);
        assert(bitCount(node->arcBitVector) == node->numArcs);
        qsort(node->arcs, node->numArcs, sizeof(ARC*), compare_arcs);
        for (j = 0; j < node->numArcs; j++) {
            nodeIdx = node->arcs[j]->destination->serializedIndex;
            assert(nodeIdx > 0 && nodeIdx < allocStates);
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

    // allocArcs = 0;
    // allocStates = 0;
    // NODE* newInitState = load_gaddag("out.gaddag");
    // printf("After loading, allocated arcs: %d, states %d\n", allocArcs,
    //        allocStates);
    // save_gaddag("out2.gaddag");
}
/**********************************************************/



NODE* load_gaddag(char* filename) {
    printf("Loading %s\n", filename);
    // loads a gaddag generated by gen_gaddag above and returns the
    // initial NODE*
    uint32_t nodeIdx;
    uint32_t* serialized;
    uint32_t num_elements;
    uint32_t num_nodes;
    int nodeCount = 0;
    int i, j, k;
    char letter;
    NODE* node;
    NODE* initialState = NULL;

    FILE* fp = fopen(filename, "rb");
    fread(&num_elements, sizeof(uint32_t), 1, fp);
    fread(&num_nodes, sizeof(uint32_t), 1, fp);
    printf("Read %d elements, %d nodes\n", num_elements, num_nodes);

    serialized = malloc(sizeof(uint32_t) * num_elements);
    assert(serialized);
    fread(serialized, sizeof(uint32_t), num_elements, fp);
    fclose(fp);
    printf("Read entire array\n");

    for (i = 0; i < num_nodes; i++) {
        createNode();
    }
    printf("Created all nodes, allocStates: %d\n", allocStates);
    i = 0;
    nodeCount = 0;
    int bc = 0;
    while (i < num_elements) {
        node = nodeArr[nodeCount];
        nodeCount++;
        node->arcBitVector = serialized[i];
        i++;
        node->letterSet = serialized[i];
        i++;
        if (!initialState) {
            initialState = node;
        }
        for (j = 0; j < 27; j++) {
            if ((1 << j) & node->arcBitVector) {
                // an arc exists for this letter
                if (j == 26) {
                    letter = SEPARATION_TOKEN;
                }
                else {
                    letter = 'A' + j;
                }
                nodeIdx = serialized[i];
                assert(nodeIdx != 0);   // no arcs should point to origin
                assert(nodeIdx < num_nodes);
                createArc(node, letter, nodeArr[nodeIdx]);
                i++;
            }
        }

    }

    free(serialized);
    return initialState;
}

