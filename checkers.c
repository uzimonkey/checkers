#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

void indent(int identation) {
  while(identation--) printf(" ");
}

void printf_indented(int indentation, const char *fmt, ...) {
  indent(indentation);

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
}

void board_display(int indentation) {
  printf_indented(indentation+2, " 1 2 3 4 5 6 7 8\n");
  printf_indented(indentation+2, "┌─┬─┬─┬─┬─┬─┬─┬─┐\n");

  for(int y = 0; y < 8; y++) {
    printf_indented(indentation, "%1d │", y+1);

    for(int x = 0; x < 8; x++)
      printf("%c│", pieces[board[y][x]]);

    printf("\n");
    if(y != 7)
      printf_indented(indentation+2, "├─┼─┼─┼─┼─┼─┼─┼─┤\n");
    else
      printf_indented(indentation+2, "└─┴─┴─┴─┴─┴─┴─┴─┘\n");
  }
}

int main(int argc, char *argv[]) {
  board_init();
  board_display(8);
}
