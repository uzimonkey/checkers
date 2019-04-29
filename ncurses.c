#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "checkers.h"

#define GAME_NAME "Checkers"

#define SQUARE_WIDTH  3
#define SQUARE_HEIGHT 1

#define BOARD_WINDOW_WIDTH  ((SQUARE_WIDTH) * (BOARD_WIDTH))
#define BOARD_WINDOW_HEIGHT ((SQUARE_HEIGHT) * (BOARD_HEIGHT))

#define LIGHT_COLOR COLOR_WHITE
#define DARK_COLOR  COLOR_GREEN

#define PAIR_EMPTY_LIGHT  10
#define PAIR_EMPTY_DARK   11
#define PAIR_PLAYER_START 12

#define PIECE_NORMAL 'o'
#define PIECE_KING   '*'

// Foreground color of players
short player_colors[NUM_PLAYERS] = {
  COLOR_WHITE,
  COLOR_BLACK
};

WINDOW *board_window;
WINDOW *status_window;

void play_game(void) {
}

// Initialize ncurses and create windows
void init_ncurses(void) {
  initscr();

  start_color();
  use_default_colors();
  assume_default_colors(-1,-1);

  raw();
  keypad(stdscr, TRUE);
  noecho();
  mousemask(BUTTON1_PRESSED, NULL);

  // Get size of terminal
  int w, h;
  getmaxyx(stdscr, h, w);

  // Make sure terminal is big enough
  if(w-2 < BOARD_WINDOW_WIDTH || h-2 < BOARD_WINDOW_HEIGHT+1) {
    endwin();
    printf("Terminal too small\n");
    exit(EXIT_FAILURE);
  }

  // Draw border and title around entire terminal
  box(stdscr, 0, 0);
  mvprintw(0, w/2 - strlen(GAME_NAME)/2, GAME_NAME);

  // Create centered board window
  board_window = newwin(
    BOARD_WINDOW_HEIGHT,
    BOARD_WINDOW_WIDTH,
    (h - BOARD_WINDOW_HEIGHT) / 2,
    (w - BOARD_WINDOW_WIDTH) / 2
  );

  // Create status window under border window
  status_window = newwin(
    1, BOARD_WINDOW_WIDTH,
    (h - BOARD_WINDOW_HEIGHT) / 2 + BOARD_WINDOW_HEIGHT,
    (w - BOARD_WINDOW_WIDTH) / 2
  );

  init_pair(PAIR_EMPTY_LIGHT, COLOR_WHITE, LIGHT_COLOR);
  init_pair(PAIR_EMPTY_DARK, COLOR_WHITE, DARK_COLOR);

  for(int pl = 0; pl < NUM_PLAYERS; pl++)
    init_pair(PAIR_PLAYER_START + pl, player_colors[pl], DARK_COLOR);

  refresh();
}

// Draw checkers square with piece
void draw_checkers_square(int x, int y, int color, char piece) {
  for(int dy = 0; dy < SQUARE_HEIGHT; dy++) {
    wmove(board_window, y*SQUARE_HEIGHT+dy, x*SQUARE_WIDTH);
    for(int dx = 0; dx < SQUARE_WIDTH; dx++) {
      char ch = (dx == SQUARE_WIDTH/2 && dy == SQUARE_HEIGHT/2) ? piece : ' ';
      waddch(board_window, color | ch);
    }
  }
}

// Draw checkers board
void draw_checkers_board() {
  for(int y = 0; y < BOARD_HEIGHT; y++) 
    for(int x = 0; x < BOARD_WIDTH; x++) {
      Piece p;
      if(get_piece(x, y, &p)) {
        draw_checkers_square(
            x, y,
            COLOR_PAIR(PAIR_PLAYER_START + p.player),
            p.king ? PIECE_KING : PIECE_NORMAL 
        );
      } else {
        draw_checkers_square(
            x, y,
            COLOR_PAIR(is_live(x,y) ? PAIR_EMPTY_DARK : PAIR_EMPTY_LIGHT),
            ' '
        );
      }
    }

  wrefresh(board_window);
}

int main(void) {
  init_ncurses();
  board_init();
  draw_checkers_board();
  mvwprintw(status_window, 0, 0, "It works!");
  wrefresh(status_window);
  getch();

  //play_game();

  endwin();
}

