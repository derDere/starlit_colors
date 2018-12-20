#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <ncurses.h>
#include <string>
#include <vector>

#define R 12
#define D (2 * R)
#define MARGIN 5
#define SCALE 2

using namespace std;

namespace game {

  struct Point {
    int x, y;
  };
  
  void initColors();
  
  class StarColor;
  class Spot;
  class Star;
  class Kristal;

  typedef vector<vector<Star*>*>* StarGrid;

  StarGrid EmptyStarGrid(int w, int h);
  
  class StarColor {
  private:
    StarColor(int s, string c, int i, int n);
  public:
    ~StarColor();
    
    int style;
    string symbol;
    int index;
    int next;

    static const StarColor red;
    static const StarColor yellow;
    static const StarColor green;
    static const StarColor blue;

    static const StarColor* colors[];
  };

  //Defines a single Char on the Screen
  class Spot {
  protected:
    Point pos;
    int style;
    bool bold;
    bool underlined;
  public:
    string symbol;
    Spot(int x, int y);
    virtual ~Spot();
    //Gets called each Frame
    virtual void update();
    //Draws the Char onto the screen
    void draw(WINDOW* win);
  };

  class Star: public Spot {
  private:
    static int rndColor();
    
    int color;

    void updateVisuals();
  public:
    Star(int x, int y);
    ~Star();
    void update();
    void hit();
  };

  class Kristal: public Spot {
  private:
    static Kristal* selection;
    bool diagonal;
    bool neighbors;
    bool selected;

    void deselect();
  public:
    Kristal(int x, int y);
    ~Kristal();
    double angle;

    void select();
    void activate(StarGrid grid);
  };
}

#endif
