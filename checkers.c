#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

char board[8][8];

struct player {
  const char *name;
  char piece;
  int king_row;
  int dir;
  int score;
} players[] = {
  {.name="White", .piece='w', .king_row=7, .dir=1},
  {.name="Black", .piece='b', .king_row=0, .dir=-1}
};

#define NUM_PLAYERS (sizeof(players)/sizeof(players[0]))
  

// Initialize board and place starting pieces
void board_init() {
  memset(&board, ' ', 8*8);

  for(int y = 0; y < 8; y++) {
    for(int x = 0; x < 8; x++) {
      if(y%2 == x%2) continue; // Skip red spaces

      // TODO
      if(y < 3) board[y][x] = players[0].piece;
      else if(y >= 5) board[y][x] = players[1].piece;
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
      printf("%c│", board[y][x]);

    printf("\n");
    if(y != 7)
      printf_indented(indentation+2, "├─┼─┼─┼─┼─┼─┼─┼─┤\n");
    else
      printf_indented(indentation+2, "└─┴─┴─┴─┴─┴─┴─┴─┘\n");
  }
}

// Return sign of x (-1, 0 or 1)
int sign(int x) {
  return -(x<0) + (x>0);
}

// Determine if a move is valid
bool valid_move(int x1, int y1, int x2, int y2, struct player *pl) {
  // Off the board
  if(x1 < 0 || x1 > 7 || y1 < 0 || y1 > 7) return false;
  if(x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7) return false;

  // Not a diagonal
  if(abs(x1-x2) != abs(y1-y2)) return false;

  // Space not empty
  if(board[y2][x2] != ' ') return false;

  char piece = board[y1][x1];
  int distance = abs(x1-x2);

  // Wrong color piece
  if(piece == ' ' || tolower(piece) != pl->piece)
    return false;

  // Correct distance
  if(distance != 1 && distance != 2) return false;

  // Correct direction
  if(piece != toupper(pl->piece) && sign(y2-y1) != pl->dir)
    return false;

  // Jump piece
  if(distance == 2 && tolower(board[(y1+y2)/2][(x1+x2)/2]) == pl->piece)
    return false;

  return true;
}

// Perform a move, capture piece if jumped
void move(int x1, int y1, int x2, int y2) {
  board[y2][x2] = board[y1][x1];
  board[y1][x1] = ' ';
  if(abs(x1-x2) == 2)
    board[(y2+y1)/2][(x2+x1)/2] = ' ';
}

// Count the pieces each player has on board
void update_player_score() {
  for(int y = 0; y < 7; y++) {
    for(int x = 0; x < y; x++) {
      int p = board[y][x];
      for(int i = 0; i < NUM_PLAYERS; i++)
        players[i].score += tolower(p) == players[i].piece;
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
  for(int i = 0; i < NUM_PLAYERS; i++)
    players[i].score = 0;

  int turn = 0;
  while(1) {
    board_display(4);

    printf("%s, it is your turn.\n", players[turn].name);
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

    if(!valid_move(x1, y1, x2, y2, &players[turn])) {
      printf("Invalid move\n");
      continue;
    }

    move(x1, y1, x2, y2);
    if(y2 == players[turn].king_row)
      board[y2][x2] = toupper(players[turn].piece);

    update_player_score();
    for(int i = 0; i < NUM_PLAYERS; i++)
      if(players[i].score == 0) {
        printf("%s loses!\n", players[i].name);
        return;
      }

    turn = (turn+1) % NUM_PLAYERS;
  }
}

int main(int argc, char *argv[]) {
  play_game();
}
