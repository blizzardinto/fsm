#ifndef CGDI_H
#define CGDI_H
#include <windows.h>
#include <string>
#include <vector>
#include <cassert>

#include "Vector2D.h"


//------------------------------- define some colors
const int NumColors = 15;

const COLORREF colors[NumColors] =
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
#define gdi Cgdi::Instance()

class Cgdi
{
public:
  
  int NumPenColors()const{return NumColors;}

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
    dark_green,
    light_blue,
    light_grey,
    light_pink,
    hollow
  };

private:

  HPEN m_OldPen;

  //all the pens
  HPEN   m_BlackPen;
  HPEN   m_WhitePen;
  HPEN   m_RedPen;
  HPEN   m_GreenPen;
  HPEN   m_BluePen;
  HPEN   m_GreyPen;
  HPEN   m_PinkPen;
  HPEN   m_OrangePen;
  HPEN   m_YellowPen;
  HPEN   m_PurplePen;
  HPEN   m_BrownPen;
  
  HPEN   m_DarkGreenPen;
  HPEN   m_LightBluePen;
  HPEN   m_LightGreyPen;
  HPEN   m_LightPinkPen;

  HPEN   m_ThickBlackPen;
  HPEN   m_ThickWhitePen;
  HPEN   m_ThickRedPen;
  HPEN   m_ThickGreenPen;
  HPEN   m_ThickBluePen;
  
  HBRUSH m_OldBrush;

  //all the brushes
  HBRUSH  m_RedBrush;
  HBRUSH  m_GreenBrush;
  HBRUSH  m_BlueBrush;
  HBRUSH  m_GreyBrush;
  HBRUSH  m_BrownBrush;
  HBRUSH  m_YellowBrush;
  HBRUSH  m_OrangeBrush;

  HBRUSH  m_LightBlueBrush;
  HBRUSH  m_DarkGreenBrush;

  HDC    m_hdc;

  //constructor is private
  Cgdi();

  //copy ctor and assignment should be private
  Cgdi(const Cgdi&);
  Cgdi& operator=(const Cgdi&);

public:

  ~Cgdi();
  
  static Cgdi* Instance();

  void BlackPen();
  void WhitePen();
  void RedPen();
  void GreenPen();
  void BluePen();
  void GreyPen();
  void PinkPen();
  void YellowPen();
  void OrangePen();
  void PurplePen();
  void BrownPen();
  
  void DarkGreenPen();
  void LightBluePen();
  void LightGreyPen();
  void LightPinkPen();

  void ThickBlackPen();
  void ThickWhitePen();
  void ThickRedPen();
  void ThickGreenPen();
  void ThickBluePen();

  void BlackBrush();
  void WhiteBrush(); 
  void HollowBrush();
  void GreenBrush();
  void RedBrush();
  void BlueBrush();
  void GreyBrush();
  void BrownBrush();
  void YellowBrush();
  void LightBlueBrush();
  void DarkGreenBrush();
  void OrangeBrush();

  //ALWAYS call this before drawing
  void StartDrawing(HDC hdc);

  //ALWAYS call this after drawing
  void StopDrawing(HDC hdc);

  //---------------------------Text
  void TextAtPos(int x, int y, const std::string &s);
  void TextAtPos(double x, double y, const std::string &s);
  void TextAtPos(Vector2D pos, const std::string &s);

  void TransparentText();
  void OpaqueText();

  void TextColor(int color);
  void TextColor(int r, int g, int b);

  //----------------------------pixels
  void DrawDot(Vector2D pos, COLORREF color);
  void DrawDot(int x, int y, COLORREF color);
  
  //-------------------------Line Drawing
  void Line(Vector2D from, Vector2D to);
  void Line(int a, int b, int x, int y);
  void Line(double a, double b, double x, double y);

  void PolyLine(const std::vector<Vector2D>& points);
  void LineWithArrow(Vector2D from, Vector2D to, double size);
  void Cross(Vector2D pos, int diameter);

  //---------------------Geometry drawing methods
  void Rect(int left, int top, int right, int bot);
  void Rect(double left, double top, double right, double bot);

  void ClosedShape(const std::vector<Vector2D> &points);

  void Circle(Vector2D pos, double radius);
  void Circle(double x, double y, double radius);
  void Circle(int x, int y, double radius);

  void SetPenColor(int color);
};

#endif