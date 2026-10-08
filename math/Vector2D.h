#ifndef S2DVECTOR_H
#define S2DVECTOR_H
#include <math.h>
#include <windows.h>
#include <iosfwd>
#include <limits>
#include "utils.h"


struct Vector2D
{
  double x;
  double y;

  Vector2D():x(0.0),y(0.0){}
  Vector2D(double a, double b):x(a),y(b){}

  //sets x and y to zero
  void Zero(){x=0.0; y=0.0;}

  //returns true if both x and y are zero
  bool isZero()const{return (x*x + y*y) < MinDouble;}

  //returns the length of the vector
  double    Length()const;

  //returns the squared length of the vector (thereby avoiding the sqrt)
  double    LengthSq()const;

  void      Normalize();

  double    Dot(const Vector2D& v2)const;

  //returns positive if v2 is clockwise of this vector,
  //negative if anticlockwise (assuming the Y axis is pointing down,
  //X axis to right like a Window app)
  int       Sign(const Vector2D& v2)const;

  //returns the vector that is perpendicular to this one.
  Vector2D  Perp()const;

  //adjusts x and y so that the length of the vector does not exceed max
  void      Truncate(double max);

  //returns the distance between this vector and th one passed as a parameter
  double    Distance(const Vector2D &v2)const;

  //squared version of above.
  double    DistanceSq(const Vector2D &v2)const;

  void      Reflect(const Vector2D& norm);

  //returns the vector that is the reverse of this vector
  Vector2D  GetReverse()const;


  //we need some overloaded operators
  const Vector2D& operator+=(const Vector2D &rhs);
  const Vector2D& operator-=(const Vector2D &rhs);
  const Vector2D& operator*=(const double& rhs);
  const Vector2D& operator/=(const double& rhs);

  bool operator==(const Vector2D& rhs)const;
  bool operator!=(const Vector2D& rhs)const;
  
};

//-----------------------------------------------------------------------some more operator overloads
Vector2D operator*(const Vector2D &lhs, double rhs);
Vector2D operator*(double lhs, const Vector2D &rhs);
Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs);
Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs);
Vector2D operator/(const Vector2D &lhs, double val);
std::ostream& operator<<(std::ostream& os, const Vector2D& rhs);
std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs);

//------------------------------------------------------------------------non member functions
enum {clockwise = 1, anticlockwise = -1};

Vector2D Vec2DNormalize(const Vector2D &v);
double Vec2DDistance(const Vector2D &v1, const Vector2D &v2);
double Vec2DDistanceSq(const Vector2D &v1, const Vector2D &v2);
double Vec2DLength(const Vector2D& v);
double Vec2DLengthSq(const Vector2D& v);
Vector2D POINTStoVector(const POINTS& p);
Vector2D POINTtoVector(const POINT& p);
POINTS VectorToPOINTS(const Vector2D& v);
POINT VectorToPOINT(const Vector2D& v);

void WrapAround(Vector2D &pos, int MaxX, int MaxY);
bool NotInsideRegion(Vector2D p, Vector2D top_left, Vector2D bot_rgt);
bool InsideRegion(Vector2D p, Vector2D top_left, Vector2D bot_rgt);
bool InsideRegion(Vector2D p, int left, int top, int right, int bottom);
bool isSecondInFOVOfFirst(Vector2D posFirst, Vector2D facingFirst, Vector2D posSecond, double fov);

#endif