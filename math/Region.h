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
    //calculate center of region
    mCenter = Vector2D( (left+right)*0.5, (top+bottom)*0.5 );

    mWidth  = fabs(right-left);
    mHeight = fabs(bottom-top);
  }

  virtual ~Region(){}

  virtual inline void     render(bool showId)const;

  //returns true if the given position lays inside the region. The
  //region modifier can be used to contract the region bounderies
  inline bool     inside(Vector2D pos, RegionModifier r)const;

  //returns a vector representing a random location
  //within the region
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
