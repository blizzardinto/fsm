#include "Cgdi.h"


//--------------------------- Instance ----------------------------------------
//
//   this class is a singleton
//-----------------------------------------------------------------------------
Cgdi* Cgdi::Instance()
{
  static Cgdi instance;
  return &instance;
}

Cgdi::Cgdi()
{
  m_BlackPen = CreatePen(PS_SOLID, 1, colors[black]);
  m_WhitePen = CreatePen(PS_SOLID, 1, colors[white]);
  m_RedPen = CreatePen(PS_SOLID, 1, colors[red]);
  m_GreenPen = CreatePen(PS_SOLID, 1, colors[green]);
  m_BluePen = CreatePen(PS_SOLID, 1, colors[blue]);
  m_GreyPen = CreatePen(PS_SOLID, 1, colors[grey]);
  m_PinkPen = CreatePen(PS_SOLID, 1, colors[pink]);
  m_YellowPen = CreatePen(PS_SOLID, 1, colors[yellow]);
  m_OrangePen = CreatePen(PS_SOLID, 1, colors[orange]);
  m_PurplePen = CreatePen(PS_SOLID, 1, colors[purple]);
  m_BrownPen = CreatePen(PS_SOLID, 1, colors[brown]);
  
  m_DarkGreenPen = CreatePen(PS_SOLID, 1, colors[dark_green]);

  m_LightBluePen = CreatePen(PS_SOLID, 1, colors[light_blue]);
  m_LightGreyPen = CreatePen(PS_SOLID, 1, colors[light_grey]);
  m_LightPinkPen = CreatePen(PS_SOLID, 1, colors[light_pink]);

  m_ThickBlackPen = CreatePen(PS_SOLID, 2, colors[black]);
  m_ThickWhitePen = CreatePen(PS_SOLID, 2, colors[white]);
  m_ThickRedPen = CreatePen(PS_SOLID, 2, colors[red]);
  m_ThickGreenPen = CreatePen(PS_SOLID, 2, colors[green]);
  m_ThickBluePen = CreatePen(PS_SOLID, 2, colors[blue]);

  m_GreenBrush = CreateSolidBrush(colors[green]);
  m_RedBrush   = CreateSolidBrush(colors[red]);
  m_BlueBrush  = CreateSolidBrush(colors[blue]);
  m_GreyBrush  = CreateSolidBrush(colors[grey]);
  m_BrownBrush = CreateSolidBrush(colors[brown]);
  m_YellowBrush = CreateSolidBrush(colors[yellow]);
  m_LightBlueBrush = CreateSolidBrush(RGB(0,255,255));
  m_DarkGreenBrush = CreateSolidBrush(colors[dark_green]);
  m_OrangeBrush = CreateSolidBrush(colors[orange]);

  m_hdc = NULL;
}

Cgdi::~Cgdi()
{
  DeleteObject(m_BlackPen);
  DeleteObject(m_WhitePen);
  DeleteObject(m_RedPen);
  DeleteObject(m_GreenPen);
  DeleteObject(m_BluePen);
  DeleteObject(m_GreyPen);
  DeleteObject(m_PinkPen);
  DeleteObject(m_OrangePen);
  DeleteObject(m_YellowPen);
  DeleteObject(m_PurplePen);
  DeleteObject(m_BrownPen);
  DeleteObject(m_OldPen);
  
  DeleteObject(m_DarkGreenPen);

  DeleteObject(m_LightBluePen);
  DeleteObject(m_LightGreyPen);
  DeleteObject(m_LightPinkPen);
  
  DeleteObject(m_ThickBlackPen);
  DeleteObject(m_ThickWhitePen);
  DeleteObject(m_ThickRedPen);
  DeleteObject(m_ThickGreenPen);
  DeleteObject(m_ThickBluePen);

  DeleteObject(m_GreenBrush);
  DeleteObject(m_RedBrush);
  DeleteObject(m_BlueBrush);
  DeleteObject(m_OldBrush);
  DeleteObject(m_GreyBrush);
  DeleteObject(m_BrownBrush);
  DeleteObject(m_LightBlueBrush);
  DeleteObject(m_YellowBrush);
  DeleteObject(m_DarkGreenBrush);
  DeleteObject(m_OrangeBrush);

}

void Cgdi::BlackPen(){if(m_hdc){SelectObject(m_hdc, m_BlackPen);}}
void Cgdi::WhitePen(){if(m_hdc){SelectObject(m_hdc, m_WhitePen);}}
void Cgdi::RedPen()  {if(m_hdc){SelectObject(m_hdc, m_RedPen);}}
void Cgdi::GreenPen(){if(m_hdc){SelectObject(m_hdc, m_GreenPen);}}
void Cgdi::BluePen() {if(m_hdc){SelectObject(m_hdc, m_BluePen);}}
void Cgdi::GreyPen() {if(m_hdc){SelectObject(m_hdc, m_GreyPen);}}
void Cgdi::PinkPen() {if(m_hdc){SelectObject(m_hdc, m_PinkPen);}}
void Cgdi::YellowPen() {if(m_hdc){SelectObject(m_hdc, m_YellowPen);}}
void Cgdi::OrangePen() {if(m_hdc){SelectObject(m_hdc, m_OrangePen);}}
void Cgdi::PurplePen() {if(m_hdc){SelectObject(m_hdc, m_PurplePen);}}
void Cgdi::BrownPen() {if(m_hdc){SelectObject(m_hdc, m_BrownPen);}}

void Cgdi::DarkGreenPen() {if(m_hdc){SelectObject(m_hdc, m_DarkGreenPen);}}
void Cgdi::LightBluePen() {if(m_hdc){SelectObject(m_hdc, m_LightBluePen);}}
void Cgdi::LightGreyPen() {if(m_hdc){SelectObject(m_hdc, m_LightGreyPen);}}
void Cgdi::LightPinkPen() {if(m_hdc){SelectObject(m_hdc, m_LightPinkPen);}}

void Cgdi::ThickBlackPen(){if(m_hdc){SelectObject(m_hdc, m_ThickBlackPen);}}
void Cgdi::ThickWhitePen(){if(m_hdc){SelectObject(m_hdc, m_ThickWhitePen);}}
void Cgdi::ThickRedPen()  {if(m_hdc){SelectObject(m_hdc, m_ThickRedPen);}}
void Cgdi::ThickGreenPen(){if(m_hdc){SelectObject(m_hdc, m_ThickGreenPen);}}
void Cgdi::ThickBluePen() {if(m_hdc){SelectObject(m_hdc, m_ThickBluePen);}}

void Cgdi::BlackBrush(){if(m_hdc)SelectObject(m_hdc, GetStockObject(BLACK_BRUSH));}
void Cgdi::WhiteBrush(){if(m_hdc)SelectObject(m_hdc, GetStockObject(WHITE_BRUSH));} 
void Cgdi::HollowBrush(){if(m_hdc)SelectObject(m_hdc, GetStockObject(HOLLOW_BRUSH));}
void Cgdi::GreenBrush(){if(m_hdc)SelectObject(m_hdc, m_GreenBrush);}
void Cgdi::RedBrush()  {if(m_hdc)SelectObject(m_hdc, m_RedBrush);}
void Cgdi::BlueBrush()  {if(m_hdc)SelectObject(m_hdc, m_BlueBrush);}
void Cgdi::GreyBrush()  {if(m_hdc)SelectObject(m_hdc, m_GreyBrush);}
void Cgdi::BrownBrush() {if(m_hdc)SelectObject(m_hdc, m_BrownBrush);}
void Cgdi::YellowBrush() {if(m_hdc)SelectObject(m_hdc, m_YellowBrush);}
void Cgdi::LightBlueBrush() {if(m_hdc)SelectObject(m_hdc, m_LightBlueBrush);}
void Cgdi::DarkGreenBrush() {if(m_hdc)SelectObject(m_hdc, m_DarkGreenBrush);}
void Cgdi::OrangeBrush() {if(m_hdc)SelectObject(m_hdc, m_OrangeBrush);}

//ALWAYS call this before drawing
void Cgdi::StartDrawing(HDC hdc)
{
  assert(m_hdc == NULL);
  
  m_hdc = hdc;

  //get the current pen
  m_OldPen = (HPEN)SelectObject(hdc, m_BlackPen);
  //select it back in
  SelectObject(hdc, m_OldPen);

  m_OldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(BLACK_BRUSH));
  SelectObject(hdc, m_OldBrush);
}

//ALWAYS call this after drawing
void Cgdi::StopDrawing(HDC hdc)
{
  assert(hdc != NULL);
  
  SelectObject(hdc, m_OldPen);
  SelectObject(hdc, m_OldBrush);

  m_hdc = NULL;
}

//---------------------------Text

void Cgdi::TextAtPos(int x, int y, const std::string &s)
{
  TextOut(m_hdc, x, y, s.c_str(), (int)s.size());
}

void Cgdi::TextAtPos(double x, double y, const std::string &s)
{
  TextOut(m_hdc, (int)x, (int)y, s.c_str(), (int)s.size());
}

void Cgdi::TextAtPos(Vector2D pos, const std::string &s)
{
  TextOut(m_hdc, (int)pos.x, (int)pos.y, s.c_str(), (int)s.size());
}

void Cgdi::TransparentText(){SetBkMode(m_hdc, TRANSPARENT);}

void Cgdi::OpaqueText(){SetBkMode(m_hdc, OPAQUE);}

void Cgdi::TextColor(int color){assert(color < NumColors); SetTextColor(m_hdc, colors[color]);}
void Cgdi::TextColor(int r, int g, int b){SetTextColor(m_hdc, RGB(r,g,b));}

//----------------------------pixels
void Cgdi::DrawDot(Vector2D pos, COLORREF color)
{
  SetPixel(m_hdc, (int)pos.x, (int)pos.y, color);
}

void Cgdi::DrawDot(int x, int y, COLORREF color)
{
  SetPixel(m_hdc, x, y, color);
}

//-------------------------Line Drawing

void Cgdi::Line(Vector2D from, Vector2D to)
{
  MoveToEx(m_hdc, (int)from.x, (int)from.y, NULL);
  LineTo(m_hdc, (int)to.x, (int)to.y);
}

void Cgdi::Line(int a, int b, int x, int y)
{
  MoveToEx(m_hdc, a, b, NULL);
  LineTo(m_hdc, x, y);
}

void Cgdi::Line(double a, double b, double x, double y)
{
  MoveToEx(m_hdc, (int)a, (int)b, NULL);
  LineTo(m_hdc, (int)x, (int)y);
}

void Cgdi::PolyLine(const std::vector<Vector2D>& points)
{
  //make sure we have at least 2 points
  if (points.size() < 2) return;

  MoveToEx(m_hdc, (int)points[0].x, (int)points[0].y, NULL);

  for (unsigned int p=1; p<points.size(); ++p)
  {
    LineTo(m_hdc, (int)points[p].x, (int)points[p].y);
  }
}

void Cgdi::LineWithArrow(Vector2D from, Vector2D to, double size)
{
  Vector2D norm = Vec2DNormalize(to-from);

  //calculate where the arrow is attached
  Vector2D CrossingPoint = to - (norm * size);
  
  //calculate the two extra points required to make the arrowhead
  Vector2D ArrowPoint1 = CrossingPoint + (norm.Perp() * 0.4f * size); 
  Vector2D ArrowPoint2 = CrossingPoint - (norm.Perp() * 0.4f * size); 

  //draw the line
  MoveToEx(m_hdc, (int)from.x, (int)from.y, NULL);
  LineTo(m_hdc, (int)CrossingPoint.x, (int)CrossingPoint.y);

  //draw the arrowhead (filled with the currently selected brush)
  POINT p[3];
  
  p[0] = VectorToPOINT(ArrowPoint1);
  p[1] = VectorToPOINT(ArrowPoint2);
  p[2] = VectorToPOINT(to);                  
                     
  SetPolyFillMode(m_hdc, WINDING);
  Polygon(m_hdc, p, 3);
}

void Cgdi::Cross(Vector2D pos, int diameter)
{
  Line((int)pos.x-diameter, (int)pos.y-diameter, (int)pos.x+diameter, (int)pos.y+diameter);
  Line((int)pos.x-diameter,(int)pos.y+diameter, (int)pos.x+diameter, (int)pos.y-diameter);
}

//---------------------Geometry drawing methods

void Cgdi::Rect(int left, int top, int right, int bot)
{
  Rectangle(m_hdc, left, top, right, bot);
}

void Cgdi::Rect(double left, double top, double right, double bot)
{
  Rectangle(m_hdc, (int)left, (int)top, (int)right, (int)bot);
}

void Cgdi::ClosedShape(const std::vector<Vector2D> &points)
{
  MoveToEx(m_hdc, (int)points[0].x, (int)points[0].y, NULL);
  
  for (unsigned int p=1; p<points.size(); ++p)
  {
    LineTo(m_hdc, (int)points[p].x, (int)points[p].y);
  }

  LineTo(m_hdc, (int)points[0].x, (int)points[0].y);
}

void Cgdi::Circle(Vector2D pos, double radius)
{
  Ellipse(m_hdc,
         (int)(pos.x-radius),
         (int)(pos.y-radius),
         (int)(pos.x+radius+1),
         (int)(pos.y+radius+1));
}

void Cgdi::Circle(double x, double y, double radius)
{
  Ellipse(m_hdc,
         (int)(x-radius),
         (int)(y-radius),
         (int)(x+radius+1),
         (int)(y+radius+1));
}

void Cgdi::Circle(int x, int y, double radius)
{
  Ellipse(m_hdc,
         (int)(x-radius),
         (int)(y-radius),
         (int)(x+radius+1),
         (int)(y+radius+1));
}

void Cgdi::SetPenColor(int color)
{
  assert (color < NumColors);
  
 switch (color)
 {
  case black:BlackPen(); return;
  case white:WhitePen(); return;
  case red: RedPen(); return;
  case green: GreenPen(); return;
  case blue: BluePen(); return;
  case pink: PinkPen(); return;
  case grey: GreyPen(); return;
  case yellow: YellowPen(); return;
  case orange: OrangePen(); return;
  case purple: PurplePen(); return;
  case brown: BrownPen(); return;
  case light_blue: LightBluePen(); return;
  case light_grey: LightGreyPen(); return;
  case light_pink: LightPinkPen(); return;
  }//end switch
}
