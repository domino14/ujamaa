#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include <stdio.h>

struct Answers answers;

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Arguments: gaddag_path anagram_str mode\n");
        printf("Modes: anagram build\n");
        return 1;
    }
    uint8_t i;
    printf("Loading gaddag...\n");
    NODE* node = load_gaddag(argv[1]);
    anagram(node, argv[2], argv[3], &answers, 1);
    for (i = 0; i < answers.num_answers; i++) {
        printf("%s ", answers.answers[i]);
    }
    printf("\n");
    // Clean up
    cleanup_answers(&answers);

    return 0;
}