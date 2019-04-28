#ifndef CHECKERS_H
#define CHECKERS_H

#include <stdbool.h>

#define BOARD_WIDTH  8
#define BOARD_HEIGHT 8
#define NUM_PLAYERS  2

typedef struct {
  int player;  // Which player owns this piece
  bool king;   // Is the piece a king?
} Piece;

typedef struct {
  const char *name; // Name of the player
  char piece;       // Piece graphic
  int king_row;     // Which row kings this player's piece
  int dir;          // Which y direction can normal pieces go
  int min_rank;     // Which row does this player's pieces start
  int max_rank;     // Which rows does this player's pieces end
} Player;

extern const Player players[NUM_PLAYERS];

bool is_live(int x, int y);
bool place_piece(int x, int y, Piece p);
bool remove_piece(int x, int y);
bool has_piece(int x, int y);
bool get_piece(int x, int y, Piece *piece);
bool promote_piece(int x, int y);
void board_init();
bool valid_move(int x1, int y1, int x2, int y2, int pl);
bool move_piece(int x1, int y1, int x2, int y2);
void get_scores(int scores[]);

#endif
