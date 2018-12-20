#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "objects.hpp"

#define STYLE_NORMAL 1
#define STYLE_RED 2
#define STYLE_YELLOW 3
#define STYLE_GREEN 4
#define STYLE_BLUE 5
#define STYLE_KRISTAL_0 6
#define STYLE_KRISTAL_1 7

using namespace std;


void game::initColors() {
  srand(time(NULL));
    
  init_pair(STYLE_NORMAL,    COLOR_WHITE,   COLOR_BLACK);
  init_pair(STYLE_RED,       COLOR_RED,     COLOR_BLACK);
  init_pair(STYLE_YELLOW,    COLOR_YELLOW,  COLOR_BLACK);
  init_pair(STYLE_GREEN,     COLOR_GREEN,   COLOR_BLACK);
  init_pair(STYLE_BLUE,      COLOR_BLUE,    COLOR_BLACK);
  init_pair(STYLE_KRISTAL_0, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(STYLE_KRISTAL_1, COLOR_WHITE,   COLOR_MAGENTA);
}


game::StarGrid game::EmptyStarGrid(int w, int h) {
  game::StarGrid result = new vector<vector<game::Star*>*>();
  for (int i = 0; i < w; i++) {
    vector<game::Star*>* line = new vector<game::Star*>();
    for (int j = 0; j < h; j++) {
      line->push_back(NULL);
    }
    result->push_back(line);
  }
  return result;
}


// StarColor_Class ==================================================
game::StarColor::StarColor(int s, string c, int i, int n) {
  this->style = s;
  this->symbol = c;
  this->index = i;
  this->next = n;
}
game::StarColor::~StarColor() {}

/*const game::StarColor game::StarColor::red(STYLE_RED, ".", 0, 1);
const game::StarColor game::StarColor::yellow(STYLE_YELLOW, ":", 1, 2);
const game::StarColor game::StarColor::green(STYLE_GREEN, "'", 2, 3);
const game::StarColor game::StarColor::blue(STYLE_BLUE, "\"", 3, 0);*/

const game::StarColor game::StarColor::red(STYLE_RED, "*", 0, 1);
const game::StarColor game::StarColor::yellow(STYLE_YELLOW, "*", 1, 2);
const game::StarColor game::StarColor::green(STYLE_GREEN, "*", 2, 3);
const game::StarColor game::StarColor::blue(STYLE_BLUE, "*", 3, 0);

const game::StarColor* game::StarColor::colors[4] = {
    &game::StarColor::red,
    &game::StarColor::yellow,
    &game::StarColor::green,
    &game::StarColor::blue
};


// Spot_Class ==================================================
game::Spot::Spot(int x, int y) {
  this->style = STYLE_NORMAL;
  this->pos = {x,y};
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
  
  mvwaddstr(win, this->pos.y + MARGIN, (SCALE * this->pos.x) + MARGIN, this->symbol.c_str());
  
  wattroff(win,COLOR_PAIR(this->style));
  if(this->underlined) wattroff(win, A_UNDERLINE);
  if(this->bold) wattroff(win, A_BOLD);
}


// Star_Class ==================================================
int game::Star::rndColor() {
  return rand() % 4;
}

game::Star::Star(int x, int y): Spot(x,y) {
  //this->bold = true;
  this->color = 3;//game::Star::rndColor();
  this->updateVisuals();
}

game::Star::~Star() {}

void game::Star::updateVisuals() {
  const StarColor* sc = StarColor::colors[this->color];
  this->style = sc->style;
  this->symbol = sc->symbol;
}

void game::Star::update() {
  this->updateVisuals();
}

void game::Star::hit() {
  const StarColor* sc = StarColor::colors[this->color];
  this->color = sc->next;
  this->updateVisuals();
}


// Kristal_Class ==================================================
game::Kristal* game::Kristal::selection = NULL;

double AngleBetween(game::Point pos, game::Point origin) {
  return atan2(pos.y - origin.y, pos.x - origin.x) * 180 / M_PI;
}

game::Kristal::Kristal(int x, int y): Spot(x,y) {
  this->style = STYLE_KRISTAL_0;
  this->bold = true;
  this->diagonal = false;
  this->neighbors = false;
  this->angle = AngleBetween(this->pos, {R,R});

  int rnd = rand() % 3;

  switch(rnd) {
  case 0:
    this->symbol = "X";
    this->diagonal = true;
    break;
  case 1:
    this->symbol = "+";
    break;
  default:
    this->symbol = "O";
    this->neighbors = true;
    break;
  }
}

game::Kristal::~Kristal() {}

void game::Kristal::select() {
  if(game::Kristal::selection != this) {
    if(game::Kristal::selection != NULL) {
      game::Kristal::selection->deselect();
    }
    this->selected = true;
    this->style = STYLE_KRISTAL_1;
    game::Kristal::selection = this;
    this->draw(stdscr);
  }
}

void game::Kristal::deselect() {
  this->style = STYLE_KRISTAL_0;
  this->selected = false;
  this->draw(stdscr);
}

bool workStar(game::StarGrid grid, int x, int y) {
  if (x < 0) return false;
  if (x >= (int)grid->size()) return false;
  if (y < 0) return false;
  if (y >= (int)grid->at(x)->size()) return false;

  game::Star* target = grid->at(x)->at(y);
  
  if (target != NULL) {
    target->symbol = " ";
    target->draw(stdscr);
    return true;
    
  } else
    return false;
}

void game::Kristal::activate(StarGrid grid) {
  if(this->neighbors) {
    //work neighbors ----------------------------------------
    workStar(grid, this->pos.x - 1, this->pos.y - 1);
    workStar(grid, this->pos.x - 1, this->pos.y    );
    workStar(grid, this->pos.x - 1, this->pos.y + 1);
    workStar(grid, this->pos.x    , this->pos.y - 1);
    workStar(grid, this->pos.x    , this->pos.y + 1);
    workStar(grid, this->pos.x + 1, this->pos.y - 1);
    workStar(grid, this->pos.x + 1, this->pos.y    );
    workStar(grid, this->pos.x + 1, this->pos.y + 1);
    
  } else if (this->diagonal) {
    //work diagonal ----------------------------------------
    int x = this->pos.x;
    int y = this->pos.y;
    do {
      x--;
      if(!workStar(grid,x,y))break;
      y--;
    } while(workStar(grid,x,y));
    
    x = this->pos.x;
    y = this->pos.y;
    do {
      x++;
      if(!workStar(grid,x,y))break;
      y--;
    } while(workStar(grid,x,y));
    
    x = this->pos.x;
    y = this->pos.y;
    do {
      x--;
      if(!workStar(grid,x,y))break;
      y++;
    } while(workStar(grid,x,y));
    
    x = this->pos.x;
    y = this->pos.y;
    do {
      x++;
      if(!workStar(grid,x,y))break;
      y++;
    } while(workStar(grid,x,y));
    
  } else {
    //work cross over ----------------------------------------
    int x = this->pos.x;
    int y = this->pos.y;
    do {
      x--;
    } while(workStar(grid,x,y));
    
    x = this->pos.x;
    y = this->pos.y;
    do {
      x++;
    } while(workStar(grid,x,y));
    
    x = this->pos.x;
    y = this->pos.y;
    do {
      y--;
    } while(workStar(grid,x,y));
    
    x = this->pos.x;
    y = this->pos.y;
    do {
      y++;
    } while(workStar(grid,x,y));
  }
}
