#include "Cgdi.h"


//--------------------------- instance ----------------------------------------
//
//   this class is a singleton
//-----------------------------------------------------------------------------
Cgdi* Cgdi::instance()
{
  static Cgdi instance;
  return &instance;
}

Cgdi::Cgdi()
{
  mBlackPen = CreatePen(PS_SOLID, 1, colors[black]);
  mWhitePen = CreatePen(PS_SOLID, 1, colors[white]);
  mRedPen = CreatePen(PS_SOLID, 1, colors[red]);
  mGreenPen = CreatePen(PS_SOLID, 1, colors[green]);
  mBluePen = CreatePen(PS_SOLID, 1, colors[blue]);
  mGreyPen = CreatePen(PS_SOLID, 1, colors[grey]);
  mPinkPen = CreatePen(PS_SOLID, 1, colors[pink]);
  mYellowPen = CreatePen(PS_SOLID, 1, colors[yellow]);
  mOrangePen = CreatePen(PS_SOLID, 1, colors[orange]);
  mPurplePen = CreatePen(PS_SOLID, 1, colors[purple]);
  mBrownPen = CreatePen(PS_SOLID, 1, colors[brown]);

  mDarkGreenPen = CreatePen(PS_SOLID, 1, colors[darkGreen]);

  mLightBluePen = CreatePen(PS_SOLID, 1, colors[lightBlue]);
  mLightGreyPen = CreatePen(PS_SOLID, 1, colors[lightGrey]);
  mLightPinkPen = CreatePen(PS_SOLID, 1, colors[lightPink]);

  mThickBlackPen = CreatePen(PS_SOLID, 2, colors[black]);
  mThickWhitePen = CreatePen(PS_SOLID, 2, colors[white]);
  mThickRedPen = CreatePen(PS_SOLID, 2, colors[red]);
  mThickGreenPen = CreatePen(PS_SOLID, 2, colors[green]);
  mThickBluePen = CreatePen(PS_SOLID, 2, colors[blue]);

  mGreenBrush = CreateSolidBrush(colors[green]);
  mRedBrush   = CreateSolidBrush(colors[red]);
  mBlueBrush  = CreateSolidBrush(colors[blue]);
  mGreyBrush  = CreateSolidBrush(colors[grey]);
  mBrownBrush = CreateSolidBrush(colors[brown]);
  mYellowBrush = CreateSolidBrush(colors[yellow]);
  mLightBlueBrush = CreateSolidBrush(RGB(0,255,255));
  mDarkGreenBrush = CreateSolidBrush(colors[darkGreen]);
  mOrangeBrush = CreateSolidBrush(colors[orange]);

  mHdc = NULL;
}

Cgdi::~Cgdi()
{
  DeleteObject(mBlackPen);
  DeleteObject(mWhitePen);
  DeleteObject(mRedPen);
  DeleteObject(mGreenPen);
  DeleteObject(mBluePen);
  DeleteObject(mGreyPen);
  DeleteObject(mPinkPen);
  DeleteObject(mOrangePen);
  DeleteObject(mYellowPen);
  DeleteObject(mPurplePen);
  DeleteObject(mBrownPen);
  DeleteObject(mOldPen);

  DeleteObject(mDarkGreenPen);

  DeleteObject(mLightBluePen);
  DeleteObject(mLightGreyPen);
  DeleteObject(mLightPinkPen);

  DeleteObject(mThickBlackPen);
  DeleteObject(mThickWhitePen);
  DeleteObject(mThickRedPen);
  DeleteObject(mThickGreenPen);
  DeleteObject(mThickBluePen);

  DeleteObject(mGreenBrush);
  DeleteObject(mRedBrush);
  DeleteObject(mBlueBrush);
  DeleteObject(mOldBrush);
  DeleteObject(mGreyBrush);
  DeleteObject(mBrownBrush);
  DeleteObject(mLightBlueBrush);
  DeleteObject(mYellowBrush);
  DeleteObject(mDarkGreenBrush);
  DeleteObject(mOrangeBrush);

}

void Cgdi::blackPen(){if(mHdc){SelectObject(mHdc, mBlackPen);}}
void Cgdi::whitePen(){if(mHdc){SelectObject(mHdc, mWhitePen);}}
void Cgdi::redPen()  {if(mHdc){SelectObject(mHdc, mRedPen);}}
void Cgdi::greenPen(){if(mHdc){SelectObject(mHdc, mGreenPen);}}
void Cgdi::bluePen() {if(mHdc){SelectObject(mHdc, mBluePen);}}
void Cgdi::greyPen() {if(mHdc){SelectObject(mHdc, mGreyPen);}}
void Cgdi::pinkPen() {if(mHdc){SelectObject(mHdc, mPinkPen);}}
void Cgdi::yellowPen() {if(mHdc){SelectObject(mHdc, mYellowPen);}}
void Cgdi::orangePen() {if(mHdc){SelectObject(mHdc, mOrangePen);}}
void Cgdi::purplePen() {if(mHdc){SelectObject(mHdc, mPurplePen);}}
void Cgdi::brownPen() {if(mHdc){SelectObject(mHdc, mBrownPen);}}

void Cgdi::darkGreenPen() {if(mHdc){SelectObject(mHdc, mDarkGreenPen);}}
void Cgdi::lightBluePen() {if(mHdc){SelectObject(mHdc, mLightBluePen);}}
void Cgdi::lightGreyPen() {if(mHdc){SelectObject(mHdc, mLightGreyPen);}}
void Cgdi::lightPinkPen() {if(mHdc){SelectObject(mHdc, mLightPinkPen);}}

void Cgdi::thickBlackPen(){if(mHdc){SelectObject(mHdc, mThickBlackPen);}}
void Cgdi::thickWhitePen(){if(mHdc){SelectObject(mHdc, mThickWhitePen);}}
void Cgdi::thickRedPen()  {if(mHdc){SelectObject(mHdc, mThickRedPen);}}
void Cgdi::thickGreenPen(){if(mHdc){SelectObject(mHdc, mThickGreenPen);}}
void Cgdi::thickBluePen() {if(mHdc){SelectObject(mHdc, mThickBluePen);}}

void Cgdi::blackBrush(){if(mHdc)SelectObject(mHdc, GetStockObject(BLACK_BRUSH));}
void Cgdi::whiteBrush(){if(mHdc)SelectObject(mHdc, GetStockObject(WHITE_BRUSH));}
void Cgdi::hollowBrush(){if(mHdc)SelectObject(mHdc, GetStockObject(HOLLOW_BRUSH));}
void Cgdi::greenBrush(){if(mHdc)SelectObject(mHdc, mGreenBrush);}
void Cgdi::redBrush()  {if(mHdc)SelectObject(mHdc, mRedBrush);}
void Cgdi::blueBrush()  {if(mHdc)SelectObject(mHdc, mBlueBrush);}
void Cgdi::greyBrush()  {if(mHdc)SelectObject(mHdc, mGreyBrush);}
void Cgdi::brownBrush() {if(mHdc)SelectObject(mHdc, mBrownBrush);}
void Cgdi::yellowBrush() {if(mHdc)SelectObject(mHdc, mYellowBrush);}
void Cgdi::lightBlueBrush() {if(mHdc)SelectObject(mHdc, mLightBlueBrush);}
void Cgdi::darkGreenBrush() {if(mHdc)SelectObject(mHdc, mDarkGreenBrush);}
void Cgdi::orangeBrush() {if(mHdc)SelectObject(mHdc, mOrangeBrush);}

//ALWAYS call this before drawing
void Cgdi::startDrawing(HDC hdc)
{
  assert(mHdc == NULL);

  mHdc = hdc;

  //get the current pen
  mOldPen = (HPEN)SelectObject(hdc, mBlackPen);
  //select it back in
  SelectObject(hdc, mOldPen);

  mOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(BLACK_BRUSH));
  SelectObject(hdc, mOldBrush);
}

//ALWAYS call this after drawing
void Cgdi::stopDrawing(HDC hdc)
{
  assert(hdc != NULL);

  SelectObject(hdc, mOldPen);
  SelectObject(hdc, mOldBrush);

  mHdc = NULL;
}

//---------------------------Text

void Cgdi::textAtPos(int x, int y, const std::string &s)
{
  TextOut(mHdc, x, y, s.c_str(), (int)s.size());
}

void Cgdi::textAtPos(double x, double y, const std::string &s)
{
  TextOut(mHdc, (int)x, (int)y, s.c_str(), (int)s.size());
}

void Cgdi::textAtPos(Vector2D pos, const std::string &s)
{
  TextOut(mHdc, (int)pos.x, (int)pos.y, s.c_str(), (int)s.size());
}

void Cgdi::transparentText(){SetBkMode(mHdc, TRANSPARENT);}

void Cgdi::opaqueText(){SetBkMode(mHdc, OPAQUE);}

void Cgdi::textColor(int color){assert(color < numColors); SetTextColor(mHdc, colors[color]);}
void Cgdi::textColor(int r, int g, int b){SetTextColor(mHdc, RGB(r,g,b));}

//----------------------------pixels
void Cgdi::drawDot(Vector2D pos, COLORREF color)
{
  SetPixel(mHdc, (int)pos.x, (int)pos.y, color);
}

void Cgdi::drawDot(int x, int y, COLORREF color)
{
  SetPixel(mHdc, x, y, color);
}

//-------------------------line Drawing

void Cgdi::line(Vector2D from, Vector2D to)
{
  MoveToEx(mHdc, (int)from.x, (int)from.y, NULL);
  LineTo(mHdc, (int)to.x, (int)to.y);
}

void Cgdi::line(int a, int b, int x, int y)
{
  MoveToEx(mHdc, a, b, NULL);
  LineTo(mHdc, x, y);
}

void Cgdi::line(double a, double b, double x, double y)
{
  MoveToEx(mHdc, (int)a, (int)b, NULL);
  LineTo(mHdc, (int)x, (int)y);
}

void Cgdi::polyLine(const std::vector<Vector2D>& points)
{
  //make sure we have at least 2 points
  if (points.size() < 2) return;

  MoveToEx(mHdc, (int)points[0].x, (int)points[0].y, NULL);

  for (unsigned int p=1; p<points.size(); ++p)
  {
    LineTo(mHdc, (int)points[p].x, (int)points[p].y);
  }
}

void Cgdi::lineWithArrow(Vector2D from, Vector2D to, double size)
{
  Vector2D norm = vec2DNormalize(to-from);

  //calculate where the arrow is attached
  Vector2D crossingPoint = to - (norm * size);

  //calculate the two extra points required to make the arrowhead
  Vector2D arrowPoint1 = crossingPoint + (norm.perp() * 0.4f * size);
  Vector2D arrowPoint2 = crossingPoint - (norm.perp() * 0.4f * size);

  //draw the line
  MoveToEx(mHdc, (int)from.x, (int)from.y, NULL);
  LineTo(mHdc, (int)crossingPoint.x, (int)crossingPoint.y);

  //draw the arrowhead (filled with the currently selected brush)
  POINT p[3];

  p[0] = vectorToPoint(arrowPoint1);
  p[1] = vectorToPoint(arrowPoint2);
  p[2] = vectorToPoint(to);

  SetPolyFillMode(mHdc, WINDING);
  Polygon(mHdc, p, 3);
}

void Cgdi::cross(Vector2D pos, int diameter)
{
  line((int)pos.x-diameter, (int)pos.y-diameter, (int)pos.x+diameter, (int)pos.y+diameter);
  line((int)pos.x-diameter,(int)pos.y+diameter, (int)pos.x+diameter, (int)pos.y-diameter);
}

//---------------------Geometry drawing methods

void Cgdi::rect(int left, int top, int right, int bot)
{
  Rectangle(mHdc, left, top, right, bot);
}

void Cgdi::rect(double left, double top, double right, double bot)
{
  Rectangle(mHdc, (int)left, (int)top, (int)right, (int)bot);
}

void Cgdi::closedShape(const std::vector<Vector2D> &points)
{
  MoveToEx(mHdc, (int)points[0].x, (int)points[0].y, NULL);

  for (unsigned int p=1; p<points.size(); ++p)
  {
    LineTo(mHdc, (int)points[p].x, (int)points[p].y);
  }

  LineTo(mHdc, (int)points[0].x, (int)points[0].y);
}

void Cgdi::circle(Vector2D pos, double radius)
{
  Ellipse(mHdc,
         (int)(pos.x-radius),
         (int)(pos.y-radius),
         (int)(pos.x+radius+1),
         (int)(pos.y+radius+1));
}

void Cgdi::circle(double x, double y, double radius)
{
  Ellipse(mHdc,
         (int)(x-radius),
         (int)(y-radius),
         (int)(x+radius+1),
         (int)(y+radius+1));
}

void Cgdi::circle(int x, int y, double radius)
{
  Ellipse(mHdc,
         (int)(x-radius),
         (int)(y-radius),
         (int)(x+radius+1),
         (int)(y+radius+1));
}

void Cgdi::setPenColor(int color)
{
  assert (color < numColors);

 switch (color)
 {
  case black:blackPen(); return;
  case white:whitePen(); return;
  case red: redPen(); return;
  case green: greenPen(); return;
  case blue: bluePen(); return;
  case pink: pinkPen(); return;
  case grey: greyPen(); return;
  case yellow: yellowPen(); return;
  case orange: orangePen(); return;
  case purple: purplePen(); return;
  case brown: brownPen(); return;
  case lightBlue: lightBluePen(); return;
  case lightGrey: lightGreyPen(); return;
  case lightPink: lightPinkPen(); return;
  }//end switch
}
