#include <stdlib.h>
#include <curses.h>
#include <vector>
#include <math.h>
#include <algorithm>

#include "objects.hpp"

#define APP "Starlit Colors"

using namespace std;

double deltaBetween(int x1, int y1, int x2, int y2);
bool isInRange(int x, int y);
bool sortKristalByAngle(game::Kristal* A, game::Kristal* B);
void quit();

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
  noecho();
  game::initColors();

  mvprintw(3, 5, "Neues Project: %s", APP);

  vector<game::Point*> points;
  vector<game::Star*> stars;
  vector<game::Kristal*> kristals;
  game::StarGrid grid = game::EmptyStarGrid(D,D);

  for (int y = 0; y < D; y++)
    for (int x = 0; x < D; x++)
      if(isInRange(x,y))
        points.push_back(new game::Point {x, y});

  unsigned int KristalCount = points.size() * 0.25;

  while(kristals.size() < KristalCount) {
    int rI = rand() % points.size();
    game::Point* pos = points.at(rI);
    game::Kristal* k = new game::Kristal(pos->x, pos->y);
    points.erase(points.begin() + rI);
    kristals.push_back(k);
    k->draw(stdscr);
  }

  sort(kristals.begin(), kristals.end(), sortKristalByAngle);

  int selectedIndex = 0;
  kristals.at(selectedIndex)->select();

  for (vector<game::Point*>::iterator it = points.begin(); it != points.end(); it++) {
    game::Point* p = *it;
    game::Star* s = new game::Star(p->x, p->y);
    stars.push_back(s);
    s->draw(stdscr);
    grid->at(p->x)->at(p->y) = s;
  }
  
  int inp = ' ';
  while(inp != 'q') {
    inp = getch();
    if(inp == 'n' || inp == 'p') {
      if(inp == 'n') {
        selectedIndex++;
        selectedIndex = selectedIndex % kristals.size();
      } else if (inp == 'p') {
        selectedIndex--;
        if(selectedIndex < 0)
      	selectedIndex += kristals.size();
      }
      game::Kristal* k = kristals.at(selectedIndex);
      k->select();
      //mvprintw(4,5,"Angle: %f    ", k->angle);
    } else if (inp == 'e') {
      mvprintw(6,5,"I: %d",selectedIndex);
      kristals.at(selectedIndex)->activate(grid);
    }
    //int ri = rand() % stars.size();
    //stars.at(ri)->hit();
    //stars.at(ri)->draw(stdscr);
  }

  return 0;
}

bool sortKristalByAngle(game::Kristal* A, game::Kristal* B) {
  return A->angle < B->angle;
}

bool isInRange(int x, int y) {
  double minR = R * 0.4;
  double maxR = R;
  double delta = deltaBetween(x,y,R,R);
  if((delta >= minR) && (delta < maxR))
    return true;
  return false;
}

void quit() {
  endwin();
}

double deltaBetween(int x1, int y1, int x2, int y2) {
  double minX = x1;
  double minY = y1;
  double maxX = x2;
  double maxY = y2;

  if (maxX < minX) {
    int TMP = maxX;
    maxX = minX;
    minX = TMP;
  }

  if (maxY < minY) {
    int TMP = maxY;
    maxY = minY;
    minY = TMP;
  }

  double dX = maxX - minX;
  double dY = maxY - minY;

  if (dX == 0) return dY;
  if (dY == 0) return dX;

  double delta = sqrt( (dX * dX) + (dY * dY) );

  return delta;
}
