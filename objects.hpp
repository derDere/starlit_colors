#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <ncurses.h>
#include <string>
#include <vector>

#define R 12
#define D (2 * R)
#define MARGIN 5
#define SCALE 2
#define ANI_TIME 50;
#define KRISTAL_QUANTITY 0.2

using namespace std;

namespace game {

  struct Point {
    int x, y;
  };

  int rndColor();
  
  void initColors();
  
  class StarColor;
  class StarColorCount;
  class Spot;
  class Star;
  class Kristal;

  typedef vector<vector<Star*>*>* StarGrid;
  typedef vector<vector<Kristal*>*>* KristalGrid;

  extern int startColor;
  extern int ani_t;
  extern StarColorCount* count;

  StarGrid EmptyStarGrid(int w, int h);
  KristalGrid EmptyKristalGrid(int w, int h);
  
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

  class StarColorCount {
  private:
    int count[4];
  public:
    StarColorCount();
    void add(int index);
    void sub(int index);
    bool check();
    void draw();
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
    void draw();
  };

  class Star: public Spot {
  private:
    int color;

    void updateVisuals();
  public:
    Star(int x, int y);
    ~Star();
    
    void update();
    void hit();
    void count();
    void drawBullet();
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
