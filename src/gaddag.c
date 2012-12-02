#include "gaddag.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SEPARATION_TOKEN '$'
#define TRUE 1
#define FALSE 0

struct State {
    struct Arc **arcs;
    int numArcs;
};

struct Arc {
    char letter;
    // letterSet is an array of booleans for that letter in the alphabet,
    // where 0 is A and 25 is Z
    char letterSet[27];
    struct State *destination;
};

typedef struct State STATE;
typedef struct Arc ARC;
STATE *initialState;

int allocArcs = 0, allocStates = 0;
void gaddag(void) {
    printf("I am in gaddag");
}

ARC** allocateSingleChildArc() {
    ARC** retArc;
    retArc = malloc(sizeof(ARC*));
    retArc[0] = malloc(sizeof(ARC));
    retArc[0]->destination = malloc(sizeof(STATE));
    retArc[0]->destination->arcs = NULL;
    //printf("Allocated child arc %p\n", retArc[0]);
    //printf("arc parent is at %p\n", retArc);
    allocArcs++;
    allocStates++;
    return retArc;
}

ARC* addNewChildArc(STATE* state) {
    ARC* retArc;
    retArc = malloc(sizeof(ARC));
    retArc->destination = malloc(sizeof(STATE));
    retArc->destination->arcs = NULL;
    state->numArcs++;
    // since the arc didn't exist, reallocate state->arcs
    state->arcs = realloc(state->arcs, state->numArcs * sizeof(ARC *));
    state->arcs[state->numArcs - 1] = retArc;
    allocStates++;
    allocArcs++;
    return retArc;
}

ARC* AddArc(STATE **state, char c) {
    // adds an arc from state for c (if one does not already exist)
    // and resets state to the node this arc leads to.
    // every state has an array of Arc pointers. we need to create the
    // Array if it doesn't exist.
   // printf("Adding Arc for state %p, char %c\n", *state, c);
    int i;
    char arcExists = FALSE;
    ARC* arc;
    if ((*state)->arcs == NULL) {
        (*state)->arcs = allocateSingleChildArc();
        (*state)->numArcs = 1;
    }

    for (i = 0; i < (*state)->numArcs; i++) {
        if ((*state)->arcs[i]->letter == c) {
            arcExists = TRUE;
            arc = (*state)->arcs[i];
            break;
        }
    }
    if (!arcExists) {
        // allocate ARC and DESTINATION for the ARC
        arc = addNewChildArc(*state);
    }

    arc->letter = c;
    *state = arc->destination;
    //printf("Updating state to %p\n", *state);
    return arc;
}

void AddFinalArc(STATE **state, char c1, char c2) {
    // add arc from state to c1 and add c2 to this arc's letter set
    ARC* arc;
    //printf("Called AddFinalArc for state %p, char %c, %c\n", *state, c1, c2);
    arc = AddArc(state, c1);
    arc->letterSet[toupper(c2) - 'A'] = TRUE;
}

void ForceArc(STATE **state, char c, STATE *forceState) {
    // add an arc from state to forceState for c (an error occurs if
    // an arc from st for c already exists going to any other state)
    int i, exists = FALSE;
    ARC* arc;
    //printf("Forcing Arc for state %p to state %p, char %c\n",
    //       *state, forceState,c);

    if ((*state)->arcs == NULL) {
        (*state)->arcs = allocateSingleChildArc();
        (*state)->numArcs = 1;
    }

    for (i = 0; i < (*state)->numArcs; i++) {
        // if ((*state)->arcs[i]->letter == c) &&
        //         (*state)->arcs[i]->destination == forceState) {
        if ((*state)->arcs[i]->letter == c) {
            printf("An error occurs!\n");
            exit(0);
            exists = TRUE;
        }
    }
    if (!exists) {
        arc = addNewChildArc(*state);
        free(arc->destination);
        allocStates--;
        arc->destination = forceState;
    }

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

void gen_gaddag(char* filename) {
    char words[300000][16];
    int numWords;
    int i = 0, j = 0, n = 0, m;
    STATE* st;
    STATE* forceSt;
    numWords = get_words(filename, words);
    numWords = 3;
    strcpy(words[0], "AB");
    strcpy(words[1], "BA");
    strcpy(words[2], "AAH");
    // fp = fopen("output.txt", "wb");
    // for (i = 0; i < numWords; i++) {
    //     fprintf(fp, "%s\n", words[i]);
    // }
    // fclose(fp);
    initialState = malloc(sizeof(STATE));
    initialState->arcs = NULL;
    st = initialState;

    for (i = 0; i < numWords; i++) {
        printf("%s\n", words[i]);
        // create path for anan-1...a1:
        n = strlen(words[i]);
        for (j = n - 1; j >= 2; j--) {
            AddArc(&st, words[i][j]);
        }
        AddFinalArc(&st, words[i][1], words[i][0]);

        // create path for an-1...a1$an
        st = initialState;
        for (j = n - 2; j >= 0; j--) {
            AddArc(&st, words[i][j]);
        }
        AddFinalArc(&st, SEPARATION_TOKEN, words[i][n - 1]);

        // partially minimize remaining paths
        for (m = n - 3; m >= 0; m--) {
            forceSt = st;
            st = initialState;
            for (j = m; j >= 0; j--) {
                AddArc(&st, words[i][j]);
            }
            AddArc(&st, SEPARATION_TOKEN);
            ForceArc(&st, words[i][m + 1], forceSt);
        }
    }
    printf("Allocated arcs: %d states: %d", allocArcs, allocStates);
}

// bugs in Gordon's algorithm:
// - addfinalarc should also update state (doesn't explicitly say)
// - letters should be stored in state, not arc. else we have a problem:
// some arcs pointing to the same node do not get their letterSets updated.
// (work out simple CARE example w/algorithm)
