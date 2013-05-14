#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Arguments: gaddag_path anagram_str\n");
        return 1;
    }
    printf("Loading gaddag...");
    NODE* node = load_gaddag(argv[1]);
    anagram(node, argv[2]);
    return 0;
}