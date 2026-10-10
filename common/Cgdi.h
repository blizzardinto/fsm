#ifndef CGDI_H
#define CGDI_H
#include <windows.h>
#include <string>
#include <vector>
#include <cassert>

#include "Vector2D.h"


//------------------------------- define some colors
const int numColors = 15;

const COLORREF colors[numColors] =
{
  RGB(255,0,0),
  RGB(0,0,255),
  RGB(0,255,0),
  RGB(0,0,0),
  RGB(255,200,200),
  RGB(200,200,200),
  RGB(255,255,0),
  RGB(255,170,0),
  RGB(255,0,170),
  RGB(133,90,0),
  RGB(255,255,255),
  RGB(0, 100, 0),        //dark green
  RGB(0, 255, 255),       //light blue
  RGB(200, 200, 200),     //light grey
  RGB(255, 230, 230)      //light pink
};


//make life easier on the fingers
#define gdi Cgdi::instance()

class Cgdi
{
public:

  int numPenColors()const{return numColors;}

  //enumerate some colors
  enum
  {
    red,
    blue,
    green,
    black,
    pink,
    grey,
    yellow,
    orange,
    purple,
    brown,
    white,
    darkGreen,
    lightBlue,
    lightGrey,
    lightPink,
    hollow
  };

private:

  HPEN mOldPen;

  //all the pens
  HPEN   mBlackPen;
  HPEN   mWhitePen;
  HPEN   mRedPen;
  HPEN   mGreenPen;
  HPEN   mBluePen;
  HPEN   mGreyPen;
  HPEN   mPinkPen;
  HPEN   mOrangePen;
  HPEN   mYellowPen;
  HPEN   mPurplePen;
  HPEN   mBrownPen;

  HPEN   mDarkGreenPen;
  HPEN   mLightBluePen;
  HPEN   mLightGreyPen;
  HPEN   mLightPinkPen;

  HPEN   mThickBlackPen;
  HPEN   mThickWhitePen;
  HPEN   mThickRedPen;
  HPEN   mThickGreenPen;
  HPEN   mThickBluePen;

  HBRUSH mOldBrush;

  //all the brushes
  HBRUSH  mRedBrush;
  HBRUSH  mGreenBrush;
  HBRUSH  mBlueBrush;
  HBRUSH  mGreyBrush;
  HBRUSH  mBrownBrush;
  HBRUSH  mYellowBrush;
  HBRUSH  mOrangeBrush;

  HBRUSH  mLightBlueBrush;
  HBRUSH  mDarkGreenBrush;

  HDC    mHdc;

  //function Object() { [native code] } is private
  Cgdi();

  //copy ctor and assignment should be private
  Cgdi(const Cgdi&);
  Cgdi& operator=(const Cgdi&);

public:

  ~Cgdi();

  static Cgdi* instance();

  void blackPen();
  void whitePen();
  void redPen();
  void greenPen();
  void bluePen();
  void greyPen();
  void pinkPen();
  void yellowPen();
  void orangePen();
  void purplePen();
  void brownPen();

  void darkGreenPen();
  void lightBluePen();
  void lightGreyPen();
  void lightPinkPen();

  void thickBlackPen();
  void thickWhitePen();
  void thickRedPen();
  void thickGreenPen();
  void thickBluePen();

  void blackBrush();
  void whiteBrush();
  void hollowBrush();
  void greenBrush();
  void redBrush();
  void blueBrush();
  void greyBrush();
  void brownBrush();
  void yellowBrush();
  void lightBlueBrush();
  void darkGreenBrush();
  void orangeBrush();

  //ALWAYS call this before drawing
  void startDrawing(HDC hdc);

  //ALWAYS call this after drawing
  void stopDrawing(HDC hdc);

  //---------------------------Text
  void textAtPos(int x, int y, const std::string &s);
  void textAtPos(double x, double y, const std::string &s);
  void textAtPos(Vector2D pos, const std::string &s);

  void transparentText();
  void opaqueText();

  void textColor(int color);
  void textColor(int r, int g, int b);

  //----------------------------pixels
  void drawDot(Vector2D pos, COLORREF color);
  void drawDot(int x, int y, COLORREF color);

  //-------------------------line Drawing
  void line(Vector2D from, Vector2D to);
  void line(int a, int b, int x, int y);
  void line(double a, double b, double x, double y);

  void polyLine(const std::vector<Vector2D>& points);
  void lineWithArrow(Vector2D from, Vector2D to, double size);
  void cross(Vector2D pos, int diameter);

  //---------------------Geometry drawing methods
  void rect(int left, int top, int right, int bot);
  void rect(double left, double top, double right, double bot);

  void closedShape(const std::vector<Vector2D> &points);

  void circle(Vector2D pos, double radius);
  void circle(double x, double y, double radius);
  void circle(int x, int y, double radius);

  void setPenColor(int color);
};

#endif