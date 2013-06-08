#include "../gaddag/gaddag.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int find_word(NODE* node, char* word);

int main(int argc, char **argv) {
    int i;
    if (argc < 3) {
        printf("Arguments: gaddag_path word [word2 ... wordn]\n");
        return 1;
    }
    printf("Loading gaddag...\n");
    NODE* node = load_gaddag(argv[1]);
    for (i = 2; i < argc; i++) {
        if (!find_word(node, argv[i])) {
            printf("Nope\n");
	    return 0;
	}
    }
    printf("Yep\n");
    return 0;
}

int find_word(NODE* node, char* word) {
    // Find the inverted word in the goddamn.
    int len = strlen(word);
    int i, j;
    char letter;
    int found;
    for (i = len - 1; i > 0; i--) {
        letter = toupper(word[i]);
        found = 0;
        for (j = 0; j < node->numArcs; j++) {
            if (node->arcs[j]->letter == letter) {
                // Follow this arc.
                node = node->arcs[j]->destination;
                found = 1;
                break;
            }
        }
        if (!found) {
            // Letter not found, so word isn't in gaddag.
            return 0;
        }
    }
    letter = toupper(word[0]);
    // For the very last letter, look in the letter set.
    if (node->letterSet & (1 << (letter - 'A'))) {
        return 1;
    }
    return 0;
}
