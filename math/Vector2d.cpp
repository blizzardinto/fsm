#include "Vector2D.h"
#include <cmath>
#include <fstream>
#include <limits>

//------------------------- length ---------------------------------------
//
//  returns the length of a 2D vector
//------------------------------------------------------------------------
double Vector2D::length()const
{
  return sqrt(x * x + y * y);
}


//------------------------- lengthSq -------------------------------------
//
//  returns the squared length of a 2D vector
//------------------------------------------------------------------------
double Vector2D::lengthSq()const
{
  return (x * x + y * y);
}


//------------------------- Vec2DDot -------------------------------------
//
//  calculates the dot product
//------------------------------------------------------------------------
double Vector2D::dot(const Vector2D &v2)const
{
  return x*v2.x + y*v2.y;
}

//------------------------ sign ------------------------------------------
//
//  returns positive if v2 is clockwise of this vector,
//  minus if anticlockwise (Y axis pointing down, X axis to right)
//------------------------------------------------------------------------
int Vector2D::sign(const Vector2D& v2)const
{
  if (y*v2.x > x*v2.y)
  {
    return anticlockwise;
  }
  else
  {
    return clockwise;
  }
}

//------------------------------ perp ------------------------------------
//
//  Returns a vector perpendicular to this vector
//------------------------------------------------------------------------
Vector2D Vector2D::perp()const
{
  return Vector2D(-y, x);
}

//------------------------------ distance --------------------------------
//
//  calculates the euclidean distance between two vectors
//------------------------------------------------------------------------
double Vector2D::distance(const Vector2D &v2)const
{
  double ySeparation = v2.y - y;
  double xSeparation = v2.x - x;

  return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}


//------------------------------ distanceSq ------------------------------
//
//  calculates the euclidean distance squared between two vectors
//------------------------------------------------------------------------
double Vector2D::distanceSq(const Vector2D &v2)const
{
  double ySeparation = v2.y - y;
  double xSeparation = v2.x - x;

  return ySeparation*ySeparation + xSeparation*xSeparation;
}

//----------------------------- truncate ---------------------------------
//
//  truncates a vector so that its length does not exceed max
//------------------------------------------------------------------------
void Vector2D::truncate(double max)
{
  if (this->length() > max)
  {
    this->normalize();

    *this *= max;
  }
}

//--------------------------- reflect ------------------------------------
//
//  given a normalized vector this method reflects the vector it
//  is operating upon. (like the path of a ball bouncing off a wall)
//------------------------------------------------------------------------
void Vector2D::reflect(const Vector2D& norm)
{
  *this += 2.0 * this->dot(norm) * norm.getReverse();
}

//----------------------- getReverse ----------------------------------------
//
//  returns the vector that is the reverse of this vector
//------------------------------------------------------------------------
Vector2D Vector2D::getReverse()const
{
  return Vector2D(-this->x, -this->y);
}


//------------------------- normalize ------------------------------------
//
//  normalizes a 2D Vector
//------------------------------------------------------------------------
void Vector2D::normalize()
{
  double vectorLength = this->length();

  if (vectorLength > std::numeric_limits<double>::epsilon())
  {
    this->x /= vectorLength;
    this->y /= vectorLength;
  }
}

//------------------------- member operator overloads ---------------------
const Vector2D& Vector2D::operator+=(const Vector2D &rhs)
{
  x += rhs.x;
  y += rhs.y;

  return *this;
}

const Vector2D& Vector2D::operator-=(const Vector2D &rhs)
{
  x += rhs.x;
  y += rhs.y;

  return *this;
}

const Vector2D& Vector2D::operator*=(const double& rhs)
{
  x *= rhs;
  y *= rhs;

  return *this;
}

const Vector2D& Vector2D::operator/=(const double& rhs)
{
  x /= rhs;
  y /= rhs;

  return *this;
}

bool Vector2D::operator==(const Vector2D& rhs)const
{
  return (isEqual(x, rhs.x) && isEqual(y,rhs.y) );
}

bool Vector2D::operator!=(const Vector2D& rhs)const
{
  return (x != rhs.x) || (y != rhs.y);
}


//------------------------------------------------------------------------non member functions

Vector2D vec2DNormalize(const Vector2D &v)
{
  Vector2D vec = v;

  double vectorLength = vec.length();

  if (vectorLength > std::numeric_limits<double>::epsilon())
  {
    vec.x /= vectorLength;
    vec.y /= vectorLength;
  }

  return vec;
}


double vec2DDistance(const Vector2D &v1, const Vector2D &v2)
{
  double ySeparation = v2.y - v1.y;
  double xSeparation = v2.x - v1.x;

  return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}

double vec2DDistanceSq(const Vector2D &v1, const Vector2D &v2)
{
  double ySeparation = v2.y - v1.y;
  double xSeparation = v2.x - v1.x;

  return ySeparation*ySeparation + xSeparation*xSeparation;
}

double vec2DLength(const Vector2D& v)
{
  return sqrt(v.x*v.x + v.y*v.y);
}

double vec2DLengthSq(const Vector2D& v)
{
  return (v.x*v.x + v.y*v.y);
}


Vector2D pointsToVector(const POINTS& p)
{
  return Vector2D(p.x, p.y);
}

Vector2D pointToVector(const POINT& p)
{
  return Vector2D((double)p.x, (double)p.y);
}

POINTS vectorToPoints(const Vector2D& v)
{
  POINTS p;
  p.x = (short)v.x;
  p.y = (short)v.y;

  return p;
}

POINT vectorToPoint(const Vector2D& v)
{
  POINT p;
  p.x = (long)v.x;
  p.y = (long)v.y;

  return p;
}



//------------------------------------------------------------------------operator overloads
Vector2D operator*(const Vector2D &lhs, double rhs)
{
  Vector2D result(lhs);
  result *= rhs;
  return result;
}

Vector2D operator*(double lhs, const Vector2D &rhs)
{
  Vector2D result(rhs);
  result *= lhs;
  return result;
}

//overload the - operator
Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs)
{
  Vector2D result(lhs);
  result.x -= rhs.x;
  result.y -= rhs.y;

  return result;
}

//overload the + operator
Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs)
{
  Vector2D result(lhs);
  result.x += rhs.x;
  result.y += rhs.y;

  return result;
}

//overload the / operator
Vector2D operator/(const Vector2D &lhs, double val)
{
  Vector2D result(lhs);
  result.x /= val;
  result.y /= val;

  return result;
}

std::ostream& operator<<(std::ostream& os, const Vector2D& rhs)
{
  os << " " << rhs.x << " " << rhs.y;
  return os;
}

std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs)
{
  is >> lhs.x >> lhs.y;
  return is;
}

///////////////////////////////////////////////////////////////////////////////


//treats a window as a toroid
void wrapAround(Vector2D &pos, int maxX, int maxY)
{
  if (pos.x > maxX) {pos.x = 0.0;}

  if (pos.x < 0)    {pos.x = (double)maxX;}

  if (pos.y < 0)    {pos.y = (double)maxY;}

  if (pos.y > maxY) {pos.y = 0.0;}
}

//returns true if the point p is not inside the region defined by topLeft
//and botRgt
bool notInsideRegion(Vector2D p,
                     Vector2D topLeft,
                     Vector2D botRgt)
{
  return (p.x < topLeft.x) || (p.x > botRgt.x) ||
         (p.y < topLeft.y) || (p.y > botRgt.y);
}

bool insideRegion(Vector2D p,
                  Vector2D topLeft,
                  Vector2D botRgt)
{
  return !((p.x < topLeft.x) || (p.x > botRgt.x) ||
         (p.y < topLeft.y) || (p.y > botRgt.y));
}

bool insideRegion(Vector2D p, int left, int top, int right, int bottom)
{
  return !( (p.x < left) || (p.x > right) || (p.y < top) || (p.y > bottom) );
}

//------------------ isSecondInFovOfFirst -------------------------------------
//
//  returns true if the target position is in the field of view of the entity
//  positioned at posFirst facing in facingFirst
//-----------------------------------------------------------------------------
bool isSecondInFovOfFirst(Vector2D posFirst,
                          Vector2D facingFirst,
                          Vector2D posSecond,
                          double    fov)
{
  Vector2D toTarget = vec2DNormalize(posSecond - posFirst);

  return facingFirst.dot(toTarget) >= cos(fov/2.0);
}
