/*
 * 阅读提示：矩形区域值对象，封装边界、中心和区域编号。
 * 球队用区域安排站位，区域本身只负责坐标判断，不决定球员战术。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef REGION_H
#define REGION_H

#include <math.h>
#include <algorithm>

#include "Vector2D.h"
#include "Cgdi.h"
#include "utils.h"
#include "Stream_Utility_Functions.h"


class Region
{
public:

  enum RegionModifier{halfsize, normal};

protected:

  double        mTop;
  double        mLeft;
  double        mRight;
  double        mBottom;

  double        mWidth;
  double        mHeight;

  Vector2D     mCenter;

  int          mId;

public:

  Region():mTop(0),mBottom(0),mLeft(0),mRight(0)
  {}


  Region(double left,
         double top,
         double right,
         double bottom,
         int id = -1):mTop(top),
                        mRight(right),
                        mLeft(left),
                        mBottom(bottom),
                        mId(id)
  {
    // 根据上下左右边界计算区域中心。
    mCenter = Vector2D( (left+right)*0.5, (top+bottom)*0.5 );

    mWidth  = fabs(right-left);
    mHeight = fabs(bottom-top);
  }

  virtual ~Region(){}

  virtual inline void     render(bool showId)const;

  // 判断坐标是否在区域内；模式参数可选择缩小后的内部区域。
  inline bool     inside(Vector2D pos, RegionModifier r)const;

  // 返回区域内的随机坐标。
  inline Vector2D getRandomPosition()const;

  //-------------------------------
  double     top()const{return mTop;}
  double     bottom()const{return mBottom;}
  double     left()const{return mLeft;}
  double     right()const{return mRight;}
  double     width()const{return fabs(mRight - mLeft);}
  double     height()const{return fabs(mTop - mBottom);}
  double     length()const{return std::max(width(), height());}
  double     breadth()const{return std::min(width(), height());}

  Vector2D  center()const{return mCenter;}
  int       id()const{return mId;}

};



inline Vector2D Region::getRandomPosition()const
{
  return Vector2D(randInRange(mLeft, mRight),
                   randInRange(mTop, mBottom));
}

inline bool Region::inside(Vector2D pos, RegionModifier r=normal)const
{
  if (r == normal)
  {
    return ((pos.x > mLeft) && (pos.x < mRight) &&
         (pos.y > mTop) && (pos.y < mBottom));
  }
  else
  {
    const double marginX = mWidth * 0.25;
    const double marginY = mHeight * 0.25;

    return ((pos.x > (mLeft+marginX)) && (pos.x < (mRight-marginX)) &&
         (pos.y > (mTop+marginY)) && (pos.y < (mBottom-marginY)));
  }

}

inline void Region::render(bool showId = 0)const
{
  gdi->hollowBrush();
  gdi->greenPen();
  gdi->rect(mLeft, mTop, mRight, mBottom);

  if (showId)
  {
    gdi->textColor(Cgdi::green);
    gdi->textAtPos(center(), ttos(id()));
  }
}


#endif
