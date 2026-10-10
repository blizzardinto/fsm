/*
 * 阅读提示：二维向量值类型的计算实现，包括长度、点积、单位化和反射。
 * 阅读公式时先确认向量表示位置还是方向；点的位置需要平移，方向向量通常只需要旋转。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "Vector2D.h"
#include <cmath>
#include <fstream>
#include <limits>

// 向量长度表示大小；位置差向量的长度就是距离。
double Vector2D::length()const
{
  return sqrt(x * x + y * y);
}


// 返回向量长度平方，比较远近时可避免开平方。
double Vector2D::lengthSq()const
{
  return (x * x + y * y);
}


// 计算点积，常用于方向投影和夹角判断。
double Vector2D::dot(const Vector2D &v2)const
{
  return x*v2.x + y*v2.y;
}

// 根据叉积符号判断转向；屏幕横轴向右、纵轴向下，顺时针为正。
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

// 返回垂直向量，可用作对象局部坐标的侧轴。
Vector2D Vector2D::perp()const
{
  return Vector2D(-y, x);
}

// 把两个坐标看作点，计算它们的直线距离。
double Vector2D::distance(const Vector2D &v2)const
{
  double ySeparation = v2.y - y;
  double xSeparation = v2.x - x;

  return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}


// 计算两个点的距离平方。
double Vector2D::distanceSq(const Vector2D &v2)const
{
  double ySeparation = v2.y - y;
  double xSeparation = v2.x - x;

  return ySeparation*ySeparation + xSeparation*xSeparation;
}

// 保留方向，将过长的向量缩短到给定最大长度。
void Vector2D::truncate(double max)
{
  if (this->length() > max)
  {
    this->normalize();

    *this *= max;
  }
}

// 根据单位法线反射向量，模拟足球撞墙反弹。
void Vector2D::reflect(const Vector2D& norm)
{
  *this += 2.0 * this->dot(norm) * norm.getReverse();
}

// 返回方向相反、长度相同的向量。
Vector2D Vector2D::getReverse()const
{
  return Vector2D(-this->x, -this->y);
}


// 非零向量除以自身长度，得到长度为一的单位向量。
void Vector2D::normalize()
{
  double vectorLength = this->length();

  if (vectorLength > std::numeric_limits<double>::epsilon())
  {
    this->x /= vectorLength;
    this->y /= vectorLength;
  }
}

// 成员运算符重载，让向量支持加减和数乘等写法。
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


// 不依赖单个对象的向量辅助函数。

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



// 非成员运算符重载，支持向量与数值的常用运算。
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

// 重载减法运算符。
Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs)
{
  Vector2D result(lhs);
  result.x -= rhs.x;
  result.y -= rhs.y;

  return result;
}

// 重载加法运算符。
Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs)
{
  Vector2D result(lhs);
  result.x += rhs.x;
  result.y += rhs.y;

  return result;
}

// 重载除法运算符。
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

// 越过一侧边界后从另一侧出现，形成首尾相连的空间。
void wrapAround(Vector2D &pos, int maxX, int maxY)
{
  if (pos.x > maxX) {pos.x = 0.0;}

  if (pos.x < 0)    {pos.x = (double)maxX;}

  if (pos.y < 0)    {pos.y = (double)maxY;}

  if (pos.y > maxY) {pos.y = 0.0;}
}

// 判断点是否在给定矩形区域外。
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

// 判断目标是否落在观察者的视野角范围内。
bool isSecondInFovOfFirst(Vector2D posFirst,
                          Vector2D facingFirst,
                          Vector2D posSecond,
                          double    fov)
{
  Vector2D toTarget = vec2DNormalize(posSecond - posFirst);

  return facingFirst.dot(toTarget) >= cos(fov/2.0);
}
