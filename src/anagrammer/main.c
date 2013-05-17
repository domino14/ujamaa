#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Arguments: gaddag_path anagram_str mode\n");
        printf("Modes: anagram build\n");
        return 1;
    }
    printf("Loading gaddag...");
    NODE* node = load_gaddag(argv[1]);
    anagram(node, argv[2], argv[3]);
    return 0;
}