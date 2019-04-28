#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include "checkers.h"

// Display the board
void board_display() {
  // Top coordinates
  printf("   ");
  for(int i = 0; i < BOARD_WIDTH; i++)
    printf("%2d  ", i+1);
  printf("\n");

  // Top line characters
  printf("  ┌");
  for(int i = 0; i < BOARD_WIDTH; i++) {
    printf("───");
    printf(i != BOARD_WIDTH-1 ? "┬" : "┐");
  }
  printf("\n");

  // Rows of pieces
  for(int y = 0; y < BOARD_HEIGHT; y++) {
    printf("%2d│", y+1);

    for(int x = 0; x < BOARD_WIDTH; x++) {
      if(has_piece(x, y)) {
        Piece p;
        get_piece(x, y, &p);
        char c = players[p.player].piece;
        if(p.king) c = toupper(c);
        printf(" %c │", c);
      } else {
        printf("   │");
      }
    }
    printf("\n");

    // Separator or bottom border
    if(y != BOARD_HEIGHT-1) {
      printf("  ├");
      for(int i = 0; i < BOARD_WIDTH; i++) {
        printf("───");
        printf(i == BOARD_WIDTH-1 ? "┤" : "┼");
      }
    } else {
      printf("  └");
      for(int i = 0; i < BOARD_WIDTH; i++) {
        printf("───");
        printf(i == BOARD_WIDTH-1 ? "┘" : "┴");
      }
    }
    printf("\n");
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

int main(int argc, char *argv[]) {
  int scores[NUM_PLAYERS] = {0};
  board_init();

  int current_player = 0;
  while(1) {
    get_scores(scores);

    // End game if only one player is left
    int players_left = 0;
    int high_score = 0;
    for(int pl = 0; pl < NUM_PLAYERS; pl++) {
      if(scores[pl] == 0) continue;
      players_left++;
      if(scores[pl] > scores[high_score]) high_score = pl;
    }

    if(players_left == 1) {
      printf("%s wins!\n", players[high_score].name);
      return 0;
    }

    // Display and get input
    board_display();

    printf("%s, it is your turn.\n", players[current_player].name);
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

    // Validate and perform move
    if(!valid_move(x1, y1, x2, y2, current_player)) {
      printf("Invalid move\n");
      continue;
    }

    bool captured = move_piece(x1, y1, x2, y2);
    if(y2 == players[current_player].king_row)
      promote_piece(x2, y2);

    if(!captured)
      current_player = (current_player+1) % NUM_PLAYERS;
  }
}
