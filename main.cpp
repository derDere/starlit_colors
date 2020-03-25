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

  // Init Curses ----------
  initscr();
  atexit(quit);
  curs_set(0);
  start_color();
  clear();
  noecho();
  cbreak();
  keypad(stdscr, true);
  mousemask(BUTTON1_CLICKED, NULL); //ALL_MOUSE_EVENTS, NULL);
  game::initColors();

  // Disabling animation for init sequence ----------
  game::ani_t = 0;

  // Print start screen
  attron(A_BOLD | A_UNDERLINE);
  mvprintw(3, 5, APP);
  attroff(A_BOLD | A_UNDERLINE);
  mvaddstr(5, 6, "Try to turn all stars into the same color.");
  mvaddstr(6, 6, "You can activate the pink cristals to change the star colors.");
  attron(A_UNDERLINE);
  mvaddstr(8, 5, "Controls:");
  attroff(A_UNDERLINE);
  mvaddstr(9,  6, "n or TAB        =  next cristal");
  mvaddstr(10, 6, "p or BACKSPACE  =  previous cristal");
  mvaddstr(11, 6, "e or ENTER      =  activate cristal");
  mvaddstr(12, 6, "q               =  quit game");
  mvaddstr(14, 6, "You can also click on the cristals with you left mouse button.");
  attron(A_BOLD);
  mvaddstr(16, 6, "[Press any key to start ...]");
  attroff(A_BOLD);
  getch();
  clear();
  
  // Print Title ----------
  mvaddstr(3, 5, APP);

  // Creating linst ----------
  vector<game::Point*> points;
  vector<game::Star*> stars;
  vector<game::Kristal*> kristals;
  game::StarGrid grid = game::EmptyStarGrid(D,D);
  game::KristalGrid kgrid = game::EmptyKristalGrid(D,D);

  // Crating Points in Circle ----------
  for (int y = 0; y < D; y++)
    for (int x = 0; x < D; x++)
      if(isInRange(x,y))
        points.push_back(new game::Point {x, y});

  // Creating Stars ----------
  for (vector<game::Point*>::iterator it = points.begin(); it != points.end(); it++) {
    game::Point* p = *it;
    game::Star* s = new game::Star(p->x, p->y);
    stars.push_back(s);
    s->draw();
    grid->at(p->x)->at(p->y) = s;
  }

  // Defining Kristal Count ----------
  unsigned int KristalCount = points.size() * KRISTAL_QUANTITY;

  // Creating Kristals at random Points ----------
  while(kristals.size() < KristalCount) {
    int rI = rand() % points.size();
    game::Point* pos = points.at(rI);
    game::Kristal* k = new game::Kristal(pos->x, pos->y);
    points.erase(points.begin() + rI);
    kristals.push_back(k);
    kgrid->at(pos->x)->at(pos->y) = k;
    // Disabling Star
    grid->at(pos->x)->at(pos->y) = NULL;
  }

  // Counting Colors ----------
  for (vector<game::Star*>::iterator it = stars.begin(); it != stars.end(); ++it) {
    (*it)->count();
  }

  // Draw and activate all Kristals ----------
  for (vector<game::Kristal*>::iterator it = kristals.begin(); it != kristals.end(); ++it) {
    (*it)->activate(grid);
    (*it)->draw();
  }

  // More Random Activations ----------
  for (int i = 0; i < 20; i++) {
    int rI = rand() % kristals.size();
    kristals.at(rI)->activate(grid);
  }

  // Sorting Kristals at angle to center ----------
  sort(kristals.begin(), kristals.end(), sortKristalByAngle);

  // Selecting first Kristal ----------
  int selectedIndex = 0;
  kristals.at(selectedIndex)->select();

  game::count->draw();

  game::ani_t = ANI_TIME;

  // Starting user input cycle ----------
  int inp = ' ';
  while(inp != 'q') {
    flushinp();
    inp = getch();
    if(inp == KEY_MOUSE) {
      MEVENT event;
      if (getmouse(&event) == OK) {
	if (
	    (event.bstate & BUTTON1_CLICKED) &&
	    (((event.x - MARGIN) % SCALE) == 0) &&
	    (event.x < ((D * SCALE) + MARGIN)) &&
	    (event.y < (D + MARGIN)) &&
	    (event.y >= MARGIN) &&
	    (event.x >= MARGIN)
	   ) {
	  int x = (event.x - MARGIN) / SCALE;
	  int y = (event.y - MARGIN);
	  game::Kristal* clickedKristal = kgrid->at(x)->at(y);
	  if(clickedKristal != NULL) {
	    clickedKristal->activate(grid);
	  }
	}
      } else {
	mvaddstr(1,0,"Nope  ");
      }
      
    } else if(inp == 'n' || inp == 'p' || inp == '\t' || inp == KEY_BACKSPACE) {
      if(inp == 'n' || inp == '\t') {
        selectedIndex++;
        selectedIndex = selectedIndex % kristals.size();
	
      } else if (inp == 'p' || inp == KEY_BACKSPACE) {
        selectedIndex--;
        if(selectedIndex < 0)
      	selectedIndex += kristals.size();
      }
      
      game::Kristal* k = kristals.at(selectedIndex);
      k->select();
      
    } else if (inp == 'e' || inp == '\n') {
      kristals.at(selectedIndex)->activate(grid);
    }
    
    if (game::count->check()) {
      clear();
      mvaddstr(3, 5, "You win");
      getch();
      break;
    }
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
