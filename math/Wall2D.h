#ifndef WALL_H
#define WALL_H
#include "Cgdi.h"
#include "Vector2D.h"
#include <fstream>


class Wall2D
{
protected:

  Vector2D    mA,
              mB,
              mN;

  void calculateNormal()
  {
    Vector2D temp = vec2DNormalize(mB - mA);

    mN.x = -temp.y;
    mN.y = temp.x;
  }

public:

  Wall2D(){}

  Wall2D(Vector2D pointA, Vector2D pointB):mA(pointA), mB(pointB)
  {
    calculateNormal();
  }

  Wall2D(Vector2D pointA, Vector2D pointB, Vector2D n):mA(pointA), mB(pointB), mN(n)
  { }

  Wall2D(std::ifstream& in){read(in);}

  virtual void render(bool renderNormals = false)const
  {
    gdi->line(mA, mB);

    //render the normals if rqd
    if (renderNormals)
    {
      int midX = (int)((mA.x+mB.x)/2);
      int midY = (int)((mA.y+mB.y)/2);

      gdi->line(midX, midY, (int)(midX+(mN.x * 5)), (int)(midY+(mN.y * 5)));
    }
  }

  Vector2D from()const  {return mA;}
  void     setFrom(Vector2D v){mA = v; calculateNormal();}

  Vector2D to()const    {return mB;}
  void     setTo(Vector2D v){mB = v; calculateNormal();}

  Vector2D normal()const{return mN;}
  void     setNormal(Vector2D n){mN = n;}

  Vector2D center()const{return (mA+mB)/2.0;}

  std::ostream& write(std::ostream& os)const
  {
    os << std::endl;
    os << from() << ",";
    os << to() << ",";
    os << normal();
    return os;
  }

 void read(std::ifstream& in)
  {
    double x,y;

    in >> x >> y;
    setFrom(Vector2D(x,y));

    in >> x >> y;
    setTo(Vector2D(x,y));

     in >> x >> y;
    setNormal(Vector2D(x,y));
  }

};

#endif