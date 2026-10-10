/*
 * 阅读提示：Windows 绘图服务，封装画笔、画刷和设备上下文操作。
 * 资源句柄需要成对获取与释放；绘图结束前恢复旧对象，避免破坏设备上下文或泄漏资源。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef CGDI_H
#define CGDI_H
#include <windows.h>
#include <string>
#include <vector>
#include <cassert>

#include "Vector2D.h"


// 定义绘图使用的颜色值。
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
  RGB(0, 100, 0),        // 深绿色。
  RGB(0, 255, 255),       // 浅蓝色。
  RGB(200, 200, 200),     // 浅灰色。
  RGB(255, 230, 230)      // 浅粉色。
};


// 用简写提供绘图服务访问入口。
#define gdi Cgdi::instance()

class Cgdi
{
public:

  int numPenColors()const{return numColors;}

  // 枚举列出可选择的颜色。
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

  // 保存不同颜色的画笔句柄。
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

  // 保存不同颜色的画刷句柄。
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

  // 构造函数私有，只允许单例入口创建对象。
  Cgdi();

  // 不允许复制或赋值，避免重复管理绘图资源。
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

  // 开始绘图前绑定设备上下文。
  void startDrawing(HDC hdc);

  // 结束绘图后恢复设备上下文中的原有对象。
  void stopDrawing(HDC hdc);

  // 文字绘制接口。
  void textAtPos(int x, int y, const std::string &s);
  void textAtPos(double x, double y, const std::string &s);
  void textAtPos(Vector2D pos, const std::string &s);

  void transparentText();
  void opaqueText();

  void textColor(int color);
  void textColor(int r, int g, int b);

  // 像素绘制接口。
  void drawDot(Vector2D pos, COLORREF color);
  void drawDot(int x, int y, COLORREF color);

  // 线段和箭头绘制接口。
  void line(Vector2D from, Vector2D to);
  void line(int a, int b, int x, int y);
  void line(double a, double b, double x, double y);

  void polyLine(const std::vector<Vector2D>& points);
  void lineWithArrow(Vector2D from, Vector2D to, double size);
  void cross(Vector2D pos, int diameter);

  // 几何图形绘制接口。
  void rect(int left, int top, int right, int bot);
  void rect(double left, double top, double right, double bot);

  void closedShape(const std::vector<Vector2D> &points);

  void circle(Vector2D pos, double radius);
  void circle(double x, double y, double radius);
  void circle(int x, int y, double radius);

  void setPenColor(int color);
};

#endif