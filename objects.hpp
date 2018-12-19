#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <ncurses.h>
#include <string>

using namespace std;

namespace game {
  
  void initColors();
  
  class StarColor;
  class Spot;
  class Star;
  class Kristal;
  
  class StarColor {
  private:
    int _index;
    int _next;
    int _style;
    string _symbol;
    StarColor(int s, string c, int i, int n);
  public:
    ~StarColor();
    
    int style();
    string symbol();
    int index();
    int next();

    static const StarColor red;
    static const StarColor yellow;
    static const StarColor green;
    static const StarColor cyan;

    static const StarColor* colors[];
  };

  //Defines a single Char on the Screen
  class Spot {
  protected:
    int style, x, y;
    string symbol;
    bool bold;
    bool underlined;
  public:
    Spot(int x, int y);
    virtual ~Spot();
    //Gets called each Frame
    virtual void update();
    //Draws the Char onto the screen
    void draw(WINDOW* win);
  };

  class Star: public Spot {
  private:
    int color;
  public:
    Star();
    ~Star();
    void update();
    void hit();
  };
}

#endif
