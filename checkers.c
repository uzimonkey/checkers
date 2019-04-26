#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

enum { EMPTY, BLACK, WHITE, BLACK_KING, WHITE_KING };
char pieces[] = {' ', 'b', 'w', 'B', 'W' };
int board[8][8];

// Initialize board and place starting pieces
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

// Print indented string
void printf_indented(int indentation, const char *fmt, ...) {
  while(indentation--) printf(" ");

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
}

// Display the board
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

// Determine if a move is valid
bool valid_move(int x1, int y1, int x2, int y2, int turn) {
  if(x1 < 0 || x1 > 7 || y1 < 0 || y1 > 7) return false;
  if(x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7) return false;
  if(abs(x1-x2) != abs(y1-y2)) return false;
  if(board[y1][x1] == EMPTY || board[y1][x1] != turn) return false;

  int distance = abs(x1-x2);
  if(distance != 1 && distance != 2) return false;

  int piece = board[y1][x1];
  if(piece == WHITE && y2 < y1) return false;
  if(piece == BLACK && y2 > y1) return false;

  if(distance == 2) {
    if(
        piece == WHITE &&
        board[y2][x2] != EMPTY &&
        board[(y1+y2)/2][(x1+x2)/2] != BLACK
      ) {
      return false;
    }

    if(
        piece == BLACK &&
        board[y2][x2] != EMPTY &&
        board[(y1+y2)/2][(x1+x2)/2] != WHITE
      ) {
      return false;
    }
  } else {
    if(board[y2][x2] != EMPTY) return false;
  }

  return true;
}

// Perform a move, capture piece if jumped
void move(int x1, int y1, int x2, int y2) {
  board[y2][x2] = board[y1][x1];
  board[y1][x1] = EMPTY;
  if(abs(x1-x2) == 2)
    board[(y2+y1)/2][(x2+x1)/2] = EMPTY;
}

// Count the pieces each player has on board
void board_score(int *white_score, int *black_score) {
  *white_score = *black_score = 0;

  for(int y = 0; y < 7; y++) {
    for(int x = 0; x < y; x++) {
      int p = board[y][x];
      *white_score += p == WHITE || p == WHITE_KING;
      *black_score += p == BLACK || p == BLACK_KING;
    }
  }
}

// Return a line of input, it's a static char* so don't free it
char *getinput(FILE *f) {
  static char *buf = NULL;
  static int buflen = 0;

  if(buf == NULL) {
    buf = malloc(16);
    buflen = 16;
  }

  char *p = buf;
  int len = buflen;
  while(1) {
    int c = fgetc(f);
    if(c == EOF || c == '\n') {
      *p = 0;
      break;
    }

    if(len <= 0) {
      len = buflen;
      buflen *= 2;
      buf = realloc(buf, buflen);
      p = buf + len;
    }

    *p++ = c;
    len--;
  }

  return buf;
}

// Play a game, return when there's a winner
void play_game() {
  board_init();

  int turn = WHITE;
  while(1) {
    board_display(4);

    printf("%s, it is your turn.\n", turn == WHITE ? "White" : "Black");
    printf("Enter move: ");
    char *line = getinput(stdin);
    int x1, x2, y1, y2;

    if(sscanf(line, "%d,%d %d,%d", &x1, &y1, &x2, &y2) != 4) {
      printf("Invalid move.\n");
      continue;
    }

    x1--;
    y1--;
    x2--;
    y2--;

    if(!valid_move(x1, y1, x2, y2, turn)) {
      printf("Invalid move\n");
      continue;
    }

    move(x1, y1, x2, y2);
    if(turn == WHITE && y2 == 7)
      board[y2][x2] = WHITE_KING;
    else if(turn == BLACK && y2 == 0)
      board[y2][x2] = BLACK_KING;

    int white_score, black_score;
    board_score(&white_score, &black_score);
    if(white_score == 0) {
      printf("Black wins!\n");
      return;
    } else if(black_score == 0) {
      printf("White wins!\n");
      return;
    }

    turn = turn == WHITE ? BLACK : WHITE;
  }
}

int main(int argc, char *argv[]) {
  play_game();
}
