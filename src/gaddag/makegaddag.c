#include "gaddag.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s [argument]\n", argv[0]);
    } else {
        gen_gaddag(argv[1]);
    }
    return 0;
}
