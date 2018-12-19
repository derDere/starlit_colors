#include <stdlib.h>
#include <curses.h>
#include "objects.hpp"

#define APP "Starlit Colors"

using namespace std;

void quit() {
  endwin();
}

/**
 * Project: starlit colors
 * Creator: phill
 * Creation Date: Mi 19. Dez 14:26:25 CET 2018
 */
int main(int argc, char* argv[]) {
  initscr();
  atexit(quit);
  curs_set(0);
  start_color();
  clear();
  game::initColors();

  mvprintw(3, 5, "Neues Project: %s", APP);

  game::Spot s(5,3);
  s.draw(stdscr);
  
  getch();

  return 0;
}
