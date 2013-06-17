#include "loadgaddag.h"

uint32_t* gaddag;

uint32_t* load_gaddag(char* filename) {
    printf("Loading %s\n", filename);
    uint32_t num_elements;
    uint32_t num_nodes;
    int nodeCount = 0;
    int i, j, k;
    char letter;

    FILE* fp = fopen(filename, "rb");
    fread(&num_elements, sizeof(uint32_t), 1, fp);
    fread(&num_nodes, sizeof(uint32_t), 1, fp);
    printf("Read %d elements, %d nodes\n", num_elements, num_nodes);

    gaddag = malloc(sizeof(uint32_t) * num_elements);
    assert(gaddag);
    fread(gaddag, sizeof(uint32_t), num_elements, fp);
    fclose(fp);
    printf("Read entire array\n");
}