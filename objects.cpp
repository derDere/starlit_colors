#include <curses.h>
#include "objects.hpp"

#define STYLE_NORMAL 1
#define STYLE_RED 2
#define STYLE_YELLOW 3
#define STYLE_GREEN 4
#define STYLE_CYAN 5
#define STYLE_KRISTAL_0 6
#define STYLE_KRISTAL_1 7

using namespace std;


void game::initColors() {
  init_pair(STYLE_NORMAL,    COLOR_WHITE,   COLOR_BLACK);
  init_pair(STYLE_RED,       COLOR_RED,     COLOR_BLACK);
  init_pair(STYLE_YELLOW,    COLOR_YELLOW,  COLOR_BLACK);
  init_pair(STYLE_GREEN,     COLOR_GREEN,   COLOR_BLACK);
  init_pair(STYLE_CYAN,      COLOR_CYAN,    COLOR_BLACK);
  init_pair(STYLE_KRISTAL_0, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(STYLE_KRISTAL_1, COLOR_WHITE,   COLOR_MAGENTA);
}


#pragma region StarColor_Class
game::StarColor::StarColor(int s, string c, int i, int n) {
  this->_style = s;
  this->_symbol = c;
  this->_index = i;
  this->_next = n;
}
game::StarColor::~StarColor() {}

int game::StarColor::style() {
  return this->_style;
}

string game::StarColor::symbol() {
  return this->_symbol;
}

int game::StarColor::index() {
  return this->_index;
}

int game::StarColor::next() {
  return this->_next;
}

const game::StarColor game::StarColor::red(STYLE_RED, ".", 0, 1);
const game::StarColor game::StarColor::yellow(STYLE_YELLOW, ":", 1, 2);
const game::StarColor game::StarColor::green(STYLE_GREEN, "\u10fb", 2, 3);
const game::StarColor game::StarColor::cyan(STYLE_CYAN, "¨", 3, 0);

const game::StarColor* game::StarColor::colors[4] = {
    &game::StarColor::red,
    &game::StarColor::yellow,
    &game::StarColor::green,
    &game::StarColor::cyan
};
#pragma endregion


#pragma region Spot_Class
game::Spot::Spot(int x, int y) {
  this->style = STYLE_NORMAL;
  this->x = x;
  this->y = y;
  this->symbol = " ";
  this->bold = false;
  this->underlined = false;
}

game::Spot::~Spot() {
}

void game::Spot::update() {}

void game::Spot::draw(WINDOW* win) {
  if(this->bold) wattron(win, A_BOLD);
  if(this->underlined) wattron(win, A_UNDERLINE);
  wattron(win,COLOR_PAIR(this->style));
  mvwaddstr(win, this->y, this->x, this->symbol.c_str());
  wattroff(win,COLOR_PAIR(this->style));
  if(this->underlined) wattroff(win, A_UNDERLINE);
  if(this->bold) wattroff(win, A_BOLD);
}
#pragma endregion
