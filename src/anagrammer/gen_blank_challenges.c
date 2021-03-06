#include "../gaddag/gaddag.h"
#include "anagrammer.h"
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct Answers answers;
void gen_blank_challenges(NODE* node, int word_length, int num, int max_sol,
                          FILE* fp);
GRand* g_rand;
char letter_distribution[101] =
    "AAAAAAAAABBCCDDDDEEEEEEEEEEEEFFGGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ??\0";
GHashTable *all_answers_hash;

int main(int argc, char **argv) {
    if (argc != 6) {
        printf("Arguments: gaddag_path num_questions max_ans_per_q length");
        printf(" filename\n");
        return 1;
    }
    g_rand = g_rand_new();
    printf("Loading gaddag...");
    NODE* node = load_gaddag(argv[1]);
    FILE* fp = fopen(argv[5], "w");
    all_answers_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
                                             NULL);
    gen_blank_challenges(node, atoi(argv[4]), atoi(argv[2]), atoi(argv[3]),
                         fp);
    g_rand_free(g_rand);
    fclose(fp);
    g_hash_table_destroy(all_answers_hash);
    return 0;
}

void gen_random_rack(char* str, int num_letters, int num_blanks) {
    uint8_t i, j;
    for (i = 0; i < num_letters; i++) {
        str[i] = letter_distribution[g_rand_int_range(g_rand, 0, 98)];
    }
    for (j = i; j < i + num_blanks; j++) {
        str[j] = '?';
    }
    str[num_blanks + num_letters] = '\0';
}


/**
 * Adds the word to the all_answers_hash, by duplicating it in memory first.
 * Note that the g_hash_table_destroy function above will free this pointer.
 * @param word The word pointer.
 */
void add_to_hash(char* word) {
    char* word_p = malloc(strlen(word));
    strcpy(word_p, word);
    g_hash_table_add(all_answers_hash, word_p);
}

/**
 * Tries to generate a blank challenge.
 * @param word_length The total length of the word.
 * @param max_sol     The number of maximum solutions allowed.
 * @param num_blanks   Number of blanks.
 * @return  1 if success, 0 if failure
 */
int try_generate_blank_challenge(int word_length, int max_sol, int num_blanks,
                             NODE* node, FILE* fp) {
    char str[16];
    int success, i;
    gen_random_rack(str, word_length - num_blanks, num_blanks);
    success = anagram(node, str, "anagram", &answers, 0, max_sol);
    if (success) {
        for (i = 0; i < answers.num_answers; i++) {
            if (g_hash_table_contains(all_answers_hash, answers.answers[i])) {
                // This answer is already in the all_answers_hash; don't
                // allow.
                printf("Generated duplicate, ignoring, %s, %s\n",
                       str, answers.answers[i]);
                cleanup_answers(&answers);
                return 0;
            }
        }
        fprintf(fp, "%s ", str);
        for (i = 0; i < answers.num_answers; i++) {
            add_to_hash(answers.answers[i]);
            fprintf(fp, "%s ", answers.answers[i]);
        }
        fprintf(fp, "\n");
        cleanup_answers(&answers);
    }
    return success;
}



void gen_blank_challenges(NODE* node, int word_length, int num, int max_sol,
                          FILE* fp) {
    uint8_t numgen, success, num_blanks;
    int num_tries;
    numgen = 0;
    num_tries = 0;
    num_blanks = 1;
    while (1) {
        success = try_generate_blank_challenge(word_length, max_sol, num_blanks,
                                               node, fp);
        numgen += success;
        num_tries += 1;
        if (numgen == num - 2) {
            num_blanks = 2;
        } else if (numgen == num) {
            break;
        }
    }
    printf("%d tries.\n", num_tries);
}

