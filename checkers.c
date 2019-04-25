#include <stdio.h>
#include <string.h>

enum { EMPTY, BLACK, WHITE, BLACK_KING, WHITE_KING };
char pieces[] = {' ', 'b', 'w', 'B', 'W' };
int board[8][8];

void board_init() {
  memset(&board, EMPTY, 8*8);

  for(int y = 0; y < 8; y++) {
    for(int x = 0; x < 8; x++) {
      if(y%2 == x%2) continue; // Skip red spaces

      if(y < 3) board[y][x] = WHITE;
      else if(y >= 5) board[y][x] = BLACK;
    }
  }
}

void board_display(int indent) {
  for(int i = 0; i < indent; i++) printf(" ");
  printf("┌─┬─┬─┬─┬─┬─┬─┬─┐\n");

  for(int y = 0; y < 8; y++) {
    for(int i = 0; i < indent; i++) printf(" ");
    printf("│");

    for(int x = 0; x < 8; x++) {
      printf("%c│", pieces[board[y][x]]);
    }

    printf("\n");
    if(y != 7) {
      for(int i = 0; i < indent; i++) printf(" ");
      printf("├─┼─┼─┼─┼─┼─┼─┼─┤\n");
    }
  }

  for(int i = 0; i < indent; i++) printf(" ");
  printf("└─┴─┴─┴─┴─┴─┴─┴─┘\n");
}

int main(int argc, char *argv[]) {
  board_init();
  board_display(8);
}

