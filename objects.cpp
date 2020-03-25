#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#include <chrono>
#include "objects.hpp"

#define STYLE_NORMAL 1
#define STYLE_RED 2
#define STYLE_YELLOW 3
#define STYLE_GREEN 4
#define STYLE_BLUE 5
#define STYLE_KRISTAL_0 6
#define STYLE_KRISTAL_1 7
#define STYLE_BULLET 8

using namespace std;


int game::startColor = 0;
int game::ani_t = ANI_TIME;
game::StarColorCount* game::count = new game::StarColorCount();


void game::initColors() {
  srand(time(NULL));

  game::startColor = game::rndColor();
  
  init_pair(STYLE_NORMAL,    COLOR_WHITE,   COLOR_BLACK);
  init_pair(STYLE_RED,       COLOR_RED,     COLOR_BLACK);
  init_pair(STYLE_YELLOW,    COLOR_YELLOW,  COLOR_BLACK);
  init_pair(STYLE_GREEN,     COLOR_GREEN,   COLOR_BLACK);
  init_pair(STYLE_BLUE,      COLOR_BLUE,    COLOR_BLACK);
  init_pair(STYLE_KRISTAL_0, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(STYLE_KRISTAL_1, COLOR_WHITE,   COLOR_MAGENTA);
  init_pair(STYLE_BULLET, COLOR_WHITE,   COLOR_WHITE);
}


int game::rndColor() {
  return rand() % 4;
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


game::KristalGrid game::EmptyKristalGrid(int w, int h) {
  game::KristalGrid result = new vector<vector<game::Kristal*>*>();
  for (int i = 0; i < w; i++) {
    vector<game::Kristal*>* line = new vector<game::Kristal*>();
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


// StarColorCount Class ==================================================
game::StarColorCount::StarColorCount() {
  this->count[0] = 0;
  this->count[1] = 0;
  this->count[2] = 0;
  this->count[3] = 0;
}

void game::StarColorCount::add(int index) {
  this->count[index] += 1;
}

void game::StarColorCount::sub(int index) {
  this->count[index] -= 1;
}

bool game::StarColorCount::check() {
  if ((this->count[0] + this->count[1] + this->count[2]) == 0) return true;
  if ((this->count[1] + this->count[2] + this->count[3]) == 0) return true;
  if ((this->count[0] + this->count[2] + this->count[3]) == 0) return true;
  if ((this->count[0] + this->count[1] + this->count[3]) == 0) return true;
  return false;
}

void game::StarColorCount::draw() {
  for (int y = 0; y < 4; y++) {
    const StarColor* sc = game::StarColor::colors[y];
    attron(COLOR_PAIR(sc->style));
    mvprintw(MARGIN + y, MARGIN, "%3d", this->count[y]);
    attroff(COLOR_PAIR(sc->style));
  }
}


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

void game::Spot::draw() {
  if(this->bold) attron(A_BOLD);
  if(this->underlined) attron(A_UNDERLINE);
  attron(COLOR_PAIR(this->style));
  
  mvaddstr(this->pos.y + MARGIN, (SCALE * this->pos.x) + MARGIN, this->symbol.c_str());
  
  attroff(COLOR_PAIR(this->style));
  if(this->underlined) attroff(A_UNDERLINE);
  if(this->bold) attroff(A_BOLD);
}


// Star_Class ==================================================
game::Star::Star(int x, int y): Spot(x,y) {
  //this->isTargeted = false;
  this->color = game::startColor;
  this->updateVisuals();
}

game::Star::~Star() {}

void game::Star::updateVisuals() {
  const StarColor* sc = StarColor::colors[this->color];
  this->style = sc->style;
  this->symbol = sc->symbol;
  this->draw();
}

void game::Star::update() {
  this->updateVisuals();
}

void game::Star::hit() {
  // Updating Color ----------
  const StarColor* sc = StarColor::colors[this->color];
  game::count->sub(this->color);
  this->color = sc->next;
  game::count->add(this->color);
  game::count->draw();
  this->updateVisuals();
}

void game::Star::drawBullet() {
  // Drawing Bullet ----------
  attron(COLOR_PAIR(STYLE_NORMAL));
  mvaddstr(this->pos.y + MARGIN, (SCALE * this->pos.x) + MARGIN, "0");
  attroff(COLOR_PAIR(STYLE_NORMAL));
  if (game::ani_t > 0)
    refresh();
}

void game::Star::count() {
  game::count->add(this->color);
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
    this->draw();
  }
}

void game::Kristal::deselect() {
  this->style = STYLE_KRISTAL_0;
  this->selected = false;
  this->draw();
}

bool workStar(game::StarGrid grid, int x, int y, bool bullet) {
  if (x < 0) return false;
  if (x >= (int)grid->size()) return false;
  if (y < 0) return false;
  if (y >= (int)grid->at(x)->size()) return false;

  game::Star* target = grid->at(x)->at(y);
  
  if (target != NULL) {
    if (bullet)
      target->drawBullet();
    else
      target->hit();
    return true;
    
  } else
    return false;
}

void game::Kristal::activate(StarGrid grid) {
  if(this->neighbors) {
    // work neighbors ----------------------------------------
    // Draw Bullets
    workStar(grid, this->pos.x + 1, this->pos.y - 1, true);
    workStar(grid, this->pos.x + 1, this->pos.y    , true);
    workStar(grid, this->pos.x + 1, this->pos.y + 1, true);
    workStar(grid, this->pos.x    , this->pos.y + 1, true);
    workStar(grid, this->pos.x - 1, this->pos.y + 1, true);
    workStar(grid, this->pos.x - 1, this->pos.y    , true);
    workStar(grid, this->pos.x - 1, this->pos.y - 1, true);
    workStar(grid, this->pos.x    , this->pos.y - 1, true);
    // Wait
    this_thread::sleep_for(chrono::milliseconds(ani_t));
    // Hit
    workStar(grid, this->pos.x + 1, this->pos.y - 1, false);
    workStar(grid, this->pos.x + 1, this->pos.y    , false);
    workStar(grid, this->pos.x + 1, this->pos.y + 1, false);
    workStar(grid, this->pos.x    , this->pos.y + 1, false);
    workStar(grid, this->pos.x - 1, this->pos.y + 1, false);
    workStar(grid, this->pos.x - 1, this->pos.y    , false);
    workStar(grid, this->pos.x - 1, this->pos.y - 1, false);
    workStar(grid, this->pos.x    , this->pos.y - 1, false);
    
  } else if (this->diagonal) {
    //work diagonal ----------------------------------------
    bool ne = true, se = true, sw = true, nw = true;
    int r = 0;
    do {
      r++;
      // Draw Bullets
      if (ne) ne = workStar(grid, this->pos.x + r, this->pos.y - r, true);
      if (se) se = workStar(grid, this->pos.x + r, this->pos.y + r, true);
      if (sw) sw = workStar(grid, this->pos.x - r, this->pos.y + r, true);
      if (nw) nw = workStar(grid, this->pos.x - r, this->pos.y - r, true);
      //Wait
      this_thread::sleep_for(chrono::milliseconds(ani_t));
      // Hit
      if (ne) ne = workStar(grid, this->pos.x + r, this->pos.y - r, false);
      if (se) se = workStar(grid, this->pos.x + r, this->pos.y + r, false);
      if (sw) sw = workStar(grid, this->pos.x - r, this->pos.y + r, false);
      if (nw) nw = workStar(grid, this->pos.x - r, this->pos.y - r, false);
      
    } while(ne || se || sw || nw);
    
  } else {
    //work cross over ----------------------------------------
    bool n = true, e = true, s = true, w = true;
    int r = 0;
    do {
      r++;
      // Draw Bullets
      if (n) n = workStar(grid, this->pos.x, this->pos.y - r, true);
      if (e) e = workStar(grid, this->pos.x + r, this->pos.y, true);
      if (s) s = workStar(grid, this->pos.x, this->pos.y + r, true);
      if (w) w = workStar(grid, this->pos.x - r, this->pos.y, true);
      // Wait
      this_thread::sleep_for(chrono::milliseconds(ani_t));
      // Hit
      if (n) n = workStar(grid, this->pos.x, this->pos.y - r, false);
      if (e) e = workStar(grid, this->pos.x + r, this->pos.y, false);
      if (s) s = workStar(grid, this->pos.x, this->pos.y + r, false);
      if (w) w = workStar(grid, this->pos.x - r, this->pos.y, false);
      
    } while (n || e || s || w);
  }
}
