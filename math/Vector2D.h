/*
 * 阅读提示：二维向量值类型，用两个数表达位置、方向、速度或力。
 * 向量没有复杂的对象所有权，通常按值复制；运算符重载让数学表达式接近纸上的公式。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
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

  // 构造函数没有返回类型，名称与类型相同；冒号后的初始化列表直接设置成员初值。
  Vector2D():x(0.0),y(0.0){}
  Vector2D(double a, double b):x(a),y(b){}

  // 将两个分量清零。
  void zero(){x=0.0; y=0.0;}

  // 判断两个分量是否都接近零。
  bool isZero()const{return (x*x + y*y) < minDouble;}

  // 返回向量长度。
  double    length()const;

  // 返回长度平方；比较大小时可避免开平方。
  double    lengthSq()const;

  // 将当前非零向量变成单位向量，会修改对象；点积函数带 const，只读取对象。
  void      normalize();

  // const 引用避免复制参数，也禁止通过这个参数修改传入向量。
  double    dot(const Vector2D& v2)const;

  // 在屏幕坐标系中判断转向，顺时针返回正号，逆时针返回负号。
  int       sign(const Vector2D& v2)const;

  // 返回与当前向量垂直的向量。
  Vector2D  perp()const;

  // 将向量长度限制到最大值，保留方向。
  void      truncate(double max);

  // 计算当前坐标与另一坐标的距离。
  double    distance(const Vector2D &v2)const;

  // 返回距离平方。
  double    distanceSq(const Vector2D &v2)const;

  void      reflect(const Vector2D& norm);

  // 返回反向向量。
  Vector2D  getReverse()const;


  // 运算符重载让向量可以使用常见算术符号。
  const Vector2D& operator+=(const Vector2D &rhs);
  const Vector2D& operator-=(const Vector2D &rhs);
  const Vector2D& operator*=(const double& rhs);
  const Vector2D& operator/=(const double& rhs);

  bool operator==(const Vector2D& rhs)const;
  bool operator!=(const Vector2D& rhs)const;

};

// 非成员算术运算符重载。
Vector2D operator*(const Vector2D &lhs, double rhs);
Vector2D operator*(double lhs, const Vector2D &rhs);
Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs);
Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs);
Vector2D operator/(const Vector2D &lhs, double val);
std::ostream& operator<<(std::ostream& os, const Vector2D& rhs);
std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs);

// 通用向量辅助函数。
enum {clockwise = 1, anticlockwise = -1};

Vector2D vec2DNormalize(const Vector2D &v);
double vec2DDistance(const Vector2D &v1, const Vector2D &v2);
double vec2DDistanceSq(const Vector2D &v1, const Vector2D &v2);
double vec2DLength(const Vector2D& v);
double vec2DLengthSq(const Vector2D& v);
Vector2D pointsToVector(const POINTS& p);
Vector2D pointToVector(const POINT& p);
POINTS vectorToPoints(const Vector2D& v);
POINT vectorToPoint(const Vector2D& v);

void wrapAround(Vector2D &pos, int maxX, int maxY);
bool notInsideRegion(Vector2D p, Vector2D topLeft, Vector2D botRgt);
bool insideRegion(Vector2D p, Vector2D topLeft, Vector2D botRgt);
bool insideRegion(Vector2D p, int left, int top, int right, int bottom);
bool isSecondInFovOfFirst(Vector2D posFirst, Vector2D facingFirst, Vector2D posSecond, double fov);

#endif