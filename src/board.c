#include "board.h"
#include "movegen.h"
#include <stdio.h>
#include <string.h>

extern struct GameState game_state;

void load_game_board() {
    FILE *fp;
    fp = fopen("config/board.txt", "rb");
    int line = 0;
    char buffer[30];
    while (fgets(buffer, 30, fp)) {
        strcpy(game_state.game_board[line], buffer);
        line++;
    }
    fclose(fp);
}