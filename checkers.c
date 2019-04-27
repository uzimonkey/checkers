#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

//
// Macros and constants
//
#define BOARD_WIDTH  8
#define BOARD_HEIGHT 8

// Live squares are the black squares
// Pieces may only legally be on live squares
#define IS_LIVE(x,y) ((x)%2 != (y)%2)


//
// Piece and board types and data
//
typedef struct {
  int player;  // Which player owns this piece
  bool king;   // Is the piece a king?
} Piece;

typedef struct {
  bool live;      // Is this a live square?
  bool has_piece; // Does this square have a piece?
  Piece piece;    // Which piece is on this square?
} Square;

Square board[BOARD_HEIGHT][BOARD_WIDTH];


//
// Player definitions
//
typedef struct {
  const char *name; // Name of the player
  char piece;       // Piece graphic
  int king_row;     // Which row kings this player's piece
  int dir;          // Which y direction can normal pieces go
  int min_rank;     // Which row does this player's pieces start
  int max_rank;     // Which rows does this player's pieces end
} Player;

Player players[] = {
  {.name="White", .piece='w', .king_row=7, .dir=1,  .min_rank=0, .max_rank=2},
  {.name="Black", .piece='b', .king_row=0, .dir=-1, .min_rank=5, .max_rank=7}
};

#define NUM_PLAYERS (sizeof(players)/sizeof(players[0]))
  

//
// Board functions
//
// Put a piece on the board
bool place_piece(int x, int y, Piece p) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return false;
  if(!board[y][x].live) return false;
  if(board[y][x].has_piece) return false;

  board[y][x].has_piece = true;
  board[y][x].piece = p;
  return true;
}

// Remove a piece from the board
bool remove_piece(int x, int y) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return false;
  if(!board[y][x].live) return false;
  if(!board[y][x].has_piece) return false;

  board[y][x].has_piece = false;
  return true;
}

// Does this square have a piece?
bool has_piece(int x, int y) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return false;
  if(!board[y][x].live) return false;
  return board[y][x].has_piece;
}

// Get the piece on this square
bool get_piece(int x, int y, Piece *piece) {
  if(!has_piece(x, y)) return false;
  *piece = board[y][x].piece;
  return true;
}

// Promote the piece to a king
bool promote_piece(int x, int y) {
  if(!has_piece(x, y)) return false;

  Piece p;
  get_piece(x, y, &p);
  bool was_king = p.king;

  remove_piece(x, y);
  p.king = true;
  place_piece(x, y, p);

  return !was_king;
}


// Initialize board and place starting pieces
void board_init() {
  for(int y = 0; y < BOARD_HEIGHT; y++) {
    for(int x = 0; x < BOARD_WIDTH; x++) {
      board[y][x] = (Square){.live=IS_LIVE(x,y), .has_piece=false};
      if(!IS_LIVE(x,y)) continue;

      for(int pl = 0; pl < NUM_PLAYERS; pl++)
        if(y >= players[pl].min_rank && y <= players[pl].max_rank)
          place_piece(x, y, (Piece){.player=pl, .king=false});
    } // for x
  } // for y
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

    for(int x = 0; x < 8; x++) {
      if(has_piece(x, y)) {
        Piece p;
        get_piece(x, y, &p);
        char c = players[p.player].piece;
        if(p.king) c = toupper(c);
        printf("%c│", c);
      } else {
        printf(" │");
      }
    }

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
bool valid_move(int x1, int y1, int x2, int y2, int pl) {
  // Off the board
  if(x1 < 0 || x1 > 7 || y1 < 0 || y1 > 7) return false;
  if(x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7) return false;

  // Dead square
  if(!IS_LIVE(x1,y1) || !IS_LIVE(x2,y2)) return false;

  // Not a diagonal
  if(abs(x1-x2) != abs(y1-y2)) return false;

  // From has a piece
  if(!has_piece(x1,y1)) return false;
  // To has no piece
  if(has_piece(x2,y2)) return false;

  Piece p;
  get_piece(x1, y1, &p);

  int distance = abs(x1-x2);

  // Wrong color piece
  if(p.player != pl)
    return false;

  // Correct distance
  if(distance != 1 && distance != 2) return false;

  // Only kings can move backwards
  if(!p.king && sign(y2-y1) != players[pl].dir)
    return false;

  // Jump piece
  if(distance == 2) {
    int midx = x1 + sign(x2-x1);
    int midy = y1 + sign(y2-y1);

    // Must jump a piece to move 2 squares
    if(!has_piece(midx, midy)) return false;

    Piece midp;
    get_piece(midx, midy, &midp);

    // Can't jump your own piece
    if(midp.player == pl) return false;
  }

  return true;
}

// Perform a move, capture piece if jumped
// This assumes the move is valid
void move(int x1, int y1, int x2, int y2) {
  Piece p;
  get_piece(x1, y1, &p);
  remove_piece(x1, y1);
  place_piece(x2, y2, p);

  if(abs(x1-x2) == 2) {
    int midx = x1 + sign(x2-x1);
    int midy = y1 + sign(y2-y1);
    remove_piece(midx, midy);
  }
}

// Count the pieces each player has on board
void update_player_score() {
//  for(int y = 0; y < 7; y++) {
//    for(int x = 0; x < y; x++) {
//      int p = board[y][x];
//      for(int i = 0; i < NUM_PLAYERS; i++)
//        players[i].score += tolower(p) == players[i].piece;
//    }
//  }
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
  // for(int i = 0; i < NUM_PLAYERS; i++)
  //   players[i].score = 0;

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

    if(!valid_move(x1, y1, x2, y2, turn)) {
      printf("Invalid move\n");
      continue;
    }

    move(x1, y1, x2, y2);
    if(y2 == players[turn].king_row)
      promote_piece(x2, y2);

    update_player_score();
    // for(int i = 0; i < NUM_PLAYERS; i++)
    //   if(players[i].score == 0) {
    //     printf("%s loses!\n", players[i].name);
    //     return;
    //   }

    turn = (turn+1) % NUM_PLAYERS;
  }
}

int main(int argc, char *argv[]) {
  play_game();
}
