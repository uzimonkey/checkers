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
#define NUM_PLAYERS  2


//
// Piece and board types and data
//
typedef struct {
  int player;  // Which player owns this piece
  bool king;   // Is the piece a king?
} Piece;

struct {
  bool has_piece; // Does this square have a piece?
  Piece piece;    // Which piece is on this square?
} board[BOARD_HEIGHT][BOARD_WIDTH];


//
// Player definitions
//
struct {
  const char *name; // Name of the player
  char piece;       // Piece graphic
  int king_row;     // Which row kings this player's piece
  int dir;          // Which y direction can normal pieces go
  int min_rank;     // Which row does this player's pieces start
  int max_rank;     // Which rows does this player's pieces end
} players[NUM_PLAYERS] = {
  {
    .name="White",
    .piece='w',
    .king_row=BOARD_HEIGHT-1,
    .dir=1,
    .min_rank=0,
    .max_rank=2
  },
  {
    .name="Black",
    .piece='b',
    .king_row=0,
    .dir=-1,
    .min_rank=BOARD_HEIGHT-3,
    .max_rank=BOARD_HEIGHT-1
  }
};


//
// Board functions
//
// Live squares are the black squares
// Pieces may only legally be on live squares
bool is_live(int x, int y) {
  return x%2 != y%2;
}

// Put a piece on the board
bool place_piece(int x, int y, Piece p) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return false;
  if(!is_live(x,y)) return false;
  if(board[y][x].has_piece) return false;

  board[y][x].has_piece = true;
  board[y][x].piece = p;
  return true;
}

// Remove a piece from the board
bool remove_piece(int x, int y) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return false;
  if(!is_live(x,y)) return false;
  if(!board[y][x].has_piece) return false;

  board[y][x].has_piece = false;
  return true;
}

// Does this square have a piece?
bool has_piece(int x, int y) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return false;
  if(!is_live(x,y)) return false;
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
      board[y][x].has_piece = false;
      if(!is_live(x,y)) continue;

      for(int pl = 0; pl < NUM_PLAYERS; pl++)
        if(y >= players[pl].min_rank && y <= players[pl].max_rank)
          place_piece(x, y, (Piece){.player=pl, .king=false});
    } // for x
  } // for y
}

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

// Return sign of x (-1, 0 or 1)
int sign(int x) {
  return -(x<0) + (x>0);
}

// Determine if a move is valid
bool valid_move(int x1, int y1, int x2, int y2, int pl) {
  // Off the board
  if(x1 < 0 || x1 >= BOARD_HEIGHT || y1 < 0 || y1 >= BOARD_HEIGHT) return false;
  if(x2 < 0 || x2 >= BOARD_WIDTH || y2 < 0 || y2 >= BOARD_WIDTH) return false;

  // Dead square
  if(!is_live(x1,y1) || !is_live(x2,y2)) return false;

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
// Returns true if move captured a piece
bool move(int x1, int y1, int x2, int y2) {
  Piece p;
  get_piece(x1, y1, &p);
  remove_piece(x1, y1);
  place_piece(x2, y2, p);

  if(abs(x1-x2) == 2) {
    int midx = x1 + sign(x2-x1);
    int midy = y1 + sign(y2-y1);
    remove_piece(midx, midy);
    return true;
  }
  
  return false;
}

// Calculate score for each player
// Assumes scores[] has as many elements as players
void get_scores(int scores[]) {
  for(int pl = 0; pl < NUM_PLAYERS; pl++)
    scores[pl] = 0;

  for(int y = 0; y < 7; y++) {
    for(int x = 0; x < y; x++) {
      Piece p;
      if(get_piece(x, y, &p))
        scores[p.player]++;
    }
  }

  int max_score = (players[0].max_rank - players[0].min_rank) * (BOARD_WIDTH/2);
  for(int pl = 0; pl < NUM_PLAYERS; pl++)
    scores[pl] = max_score - scores[pl];
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
      return;
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

    bool captured = move(x1, y1, x2, y2);
    if(y2 == players[current_player].king_row)
      promote_piece(x2, y2);

    if(!captured)
      current_player = (current_player+1) % NUM_PLAYERS;
  }
}

int main(int argc, char *argv[]) {
  play_game();
}
