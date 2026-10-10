/*
 * 阅读提示：几何算法集合，提供线段、圆、射线和多边形的相交与距离判断。
 * 这些计算不依赖球队或窗口，适合用普通函数表达；面向对象并不要求把所有算法都包装成类。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "geometry.h"

// 求射线与平面交点沿射线方向的距离；平行时返回负值。
double distanceToRayPlaneIntersection(Vector2D rayOrigin,
                                     Vector2D rayHeading,
                                     Vector2D planePoint,  // 平面上的任意一个点。
                                     Vector2D planeNormal)
{

  double d     = - planeNormal.dot(planePoint);
  double numer = planeNormal.dot(rayOrigin) + d;
  double denom = planeNormal.dot(rayHeading);

  // 射线方向与平面法线垂直时，射线平行于平面。
  if ((denom < 0.000001) && (denom > -0.000001))
  {
   return (-1.0);
  }

  return -(numer / denom);
}

// 判断点位于平面的哪一侧。
SpanType whereIsPoint(Vector2D point,
                       Vector2D pointOnPlane, // 平面上的任意一个点。
                       Vector2D planeNormal)
{
 Vector2D dir = pointOnPlane - point;

 double d = dir.dot(planeNormal);

 if (d<-0.000001)
 {
  return planeFront;
 }

 else if (d>0.000001)
 {
  return planeBackside;
 }

  return onPlane;
}


// 求射线与圆的第一个交点距离。
double getRayCircleIntersect(Vector2D rayOrigin,
                             Vector2D rayHeading,
                             Vector2D circleOrigin,
                             double  radius)
{

   Vector2D toCircle = circleOrigin-rayOrigin;
   double length      = toCircle.length();
   double v           = toCircle.dot(rayHeading);
   double d           = radius*radius - (length*length - v*v);

   // 无交点时返回负一。
   if (d < 0.0) return (-1.0);

   // 返回射线到第一个交点的距离。
   return (v - sqrt(d));
}

// 射线与圆的判别式检查：注意本实现返回判别式小于零，即无实交点，与函数名称含义相反。
bool doRayCircleIntersect(Vector2D rayOrigin,
                          Vector2D rayHeading,
                          Vector2D circleOrigin,
                          double     radius)
{

   Vector2D toCircle = circleOrigin-rayOrigin;
   double length      = toCircle.length();
   double v           = toCircle.dot(rayHeading);
   double d           = radius*radius - (length*length - v*v);

   // 判别式小于零时没有实交点；本实现此时返回真，阅读调用时不能按名称推断其语义。
   return (d < 0.0);
}


// 从圆外点向圆作两条切线，计算切点；点在圆内或圆上时返回假。算法参考 Dave Eberly。
bool getTangentPoints (Vector2D c, double radiusValue, Vector2D pointP, Vector2D& t1, Vector2D& t2)
{
  Vector2D pmC = pointP - c;
  double sqrLen = pmC.lengthSq();
  double rSqr = radiusValue*radiusValue;
  if ( sqrLen <= rSqr )
  {
      // 点在圆内或圆上，无法得到两个不同的外部切点。
      return false;
  }

  double invSqrLen = 1/sqrLen;
  double root = sqrt(fabs(sqrLen - rSqr));

  t1.x = c.x + radiusValue*(radiusValue*pmC.x - pmC.y*root)*invSqrLen;
  t1.y = c.y + radiusValue*(radiusValue*pmC.y + pmC.x*root)*invSqrLen;
  t2.x = c.x + radiusValue*(radiusValue*pmC.x + pmC.y*root)*invSqrLen;
  t2.y = c.y + radiusValue*(radiusValue*pmC.y - pmC.x*root)*invSqrLen;

  return true;
}




// 计算点到线段的最短距离；投影不在线段内时取最近端点。
double distToLineSegment(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D pointP)
{
  // 投影位于起点外侧时，最近点是端点 A。
  double dotA = (pointP.x - pointA.x)*(pointB.x - pointA.x) + (pointP.y - pointA.y)*(pointB.y - pointA.y);

  if (dotA <= 0) return vec2DDistance(pointA, pointP);

  // 投影位于终点外侧时，最近点是端点 B。
  double dotB = (pointP.x - pointB.x)*(pointA.x - pointB.x) + (pointP.y - pointB.y)*(pointA.y - pointB.y);

  if (dotB <= 0) return vec2DDistance(pointB, pointP);

  // 计算线段上距离目标点最近的投影点。
  Vector2D point = pointA + ((pointB - pointA) * dotA)/(dotA + dotB);

  // 计算目标点到投影点的距离。
  return vec2DDistance(pointP,point);
}

// 计算点到线段的距离平方，避免开平方。
double distToLineSegmentSq(Vector2D pointA,
                          Vector2D pointB,
                          Vector2D pointP)
{
  // 投影位于起点外侧时，最近点是端点 A。
  double dotA = (pointP.x - pointA.x)*(pointB.x - pointA.x) + (pointP.y - pointA.y)*(pointB.y - pointA.y);

  if (dotA <= 0) return vec2DDistanceSq(pointA, pointP);

  // 投影位于终点外侧时，最近点是端点 B。
  double dotB = (pointP.x - pointB.x)*(pointA.x - pointB.x) + (pointP.y - pointB.y)*(pointA.y - pointB.y);

  if (dotB <= 0) return vec2DDistanceSq(pointB, pointP);

  // 计算线段上距离目标点最近的投影点。
  Vector2D point = pointA + ((pointB - pointA) * dotA)/(dotA + dotB);

  // 计算目标点到投影点的距离平方。
  return vec2DDistanceSq(pointP,point);
}


// 判断两条二维线段是否相交。

bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD)
{
  double rTop = (pointA.y-c.y)*(pointD.x-c.x)-(pointA.x-c.x)*(pointD.y-c.y);
  double sTop = (pointA.y-c.y)*(pointB.x-pointA.x)-(pointA.x-c.x)*(pointB.y-pointA.y);

  double bot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);

  if (bot == 0)// 两条线段所在的直线平行。
  {
    return false;
  }

  double invBot = 1.0/bot;
  double r = rTop * invBot;
  double s = sTop * invBot;

  if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
  {
    // 交点同时在线段范围内，判定相交。
    return true;
  }

  // 交点不同时位于两条线段内，判定不相交。
  return false;
}

// 判断两条线段是否相交，并通过引用返回沿第一条线段到交点的距离。

bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD,
                        double &dist)
{

  double rTop = (pointA.y-c.y)*(pointD.x-c.x)-(pointA.x-c.x)*(pointD.y-c.y);
  double sTop = (pointA.y-c.y)*(pointB.x-pointA.x)-(pointA.x-c.x)*(pointB.y-pointA.y);

  double bot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);


  if (bot == 0)// 两条线段所在的直线平行。
  {
    if (isEqual(rTop, 0) && isEqual(sTop, 0))
    {
      return true;
    }
    return false;
  }

  double r = rTop/bot;
  double s = sTop/bot;

  if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
  {
    dist = vec2DDistance(pointA,pointB) * r;

    return true;
  }

  else
  {
    dist = 0;

    return false;
  }
}

// 判断两条线段是否相交，并返回到交点的距离和交点坐标。
bool lineIntersection2D(Vector2D   pointA,
                        Vector2D   pointB,
                        Vector2D   c,
                        Vector2D   pointD,
                        double&     dist,
                        Vector2D&  point)
{

  double rTop = (pointA.y-c.y)*(pointD.x-c.x)-(pointA.x-c.x)*(pointD.y-c.y);
  double rBot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);

  double sTop = (pointA.y-c.y)*(pointB.x-pointA.x)-(pointA.x-c.x)*(pointB.y-pointA.y);
  double sBot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);

  if ( (rBot == 0) || (sBot == 0))
  {
    // 两条直线平行，没有唯一交点。
    return false;
  }

  double r = rTop/rBot;
  double s = sTop/sBot;

  if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
  {
    dist = vec2DDistance(pointA,pointB) * r;

    point = pointA + r * (pointB - pointA);

    return true;
  }

  else
  {
    dist = 0;

    return false;
  }
}

// 检查两个多边形的边是否相交；不检查一个多边形完全包含另一个的情况。
bool objectIntersection2D(const std::vector<Vector2D>& object1,
                          const std::vector<Vector2D>& object2)
{
  // 将第一个多边形的每条边与第二个多边形的每条边分别比较。
  for (unsigned int r=0; r<object1.size()-1; ++r)
  {
    for (unsigned int t=0; t<object2.size()-1; ++t)
    {
      if (lineIntersection2D(object2[t],
                             object2[t+1],
                             object1[r],
                             object1[r+1]))
      {
        return true;
      }
    }
  }

  return false;
}

// 检查线段是否与多边形边界相交；不检查线段完全处于内部的情况。
bool segmentObjectIntersection2D(const Vector2D& pointA,
                                 const Vector2D& pointB,
                                 const std::vector<Vector2D>& object)
{
  // 将线段与多边形的每条边比较。
  for (unsigned int r=0; r<object.size()-1; ++r)
  {
    if (lineIntersection2D(pointA, pointB, object[r], object[r+1]))
    {
      return true;
    }
  }

  return false;
}


// 根据圆心距离和半径判断两个圆是否重叠。
bool twoCirclesOverlapped(double x1, double y1, double r1,
                          double x2, double y2, double r2)
{
  double distBetweenCenters = sqrt( (x1-x2) * (x1-x2) +
                                    (y1-y2) * (y1-y2));

  if ((distBetweenCenters < (r1+r2)) || (distBetweenCenters < fabs(r1-r2)))
  {
    return true;
  }

  return false;
}

// 使用圆心向量判断两个圆是否重叠。
bool twoCirclesOverlapped(Vector2D c1, double r1,
                          Vector2D c2, double r2)
{
  double distBetweenCenters = sqrt( (c1.x-c2.x) * (c1.x-c2.x) +
                                    (c1.y-c2.y) * (c1.y-c2.y));

  if ((distBetweenCenters < (r1+r2)) || (distBetweenCenters < fabs(r1-r2)))
  {
    return true;
  }

  return false;
}

// 判断一个圆是否完全包含另一个圆。
bool twoCirclesEnclosed(double x1, double y1, double r1,
                        double x2, double y2, double r2)
{
  double distBetweenCenters = sqrt( (x1-x2) * (x1-x2) +
                                    (y1-y2) * (y1-y2));

  if (distBetweenCenters < fabs(r1-r2))
  {
    return true;
  }

  return false;
}

// 计算两个圆的交点，无符合条件的重叠时返回假。算法参考：http://astronomy.swin.edu.au/~pbourke/geometry/2circle/。
bool twoCirclesIntersectionPoints(double x1, double y1, double r1,
                                  double x2, double y2, double r2,
                                  double &p3X, double &p3Y,
                                  double &p4X, double &p4Y)
{
  // 先判断两个圆是否存在重叠。
  if (!twoCirclesOverlapped(x1,y1,r1,x2,y2,r2))
  {
    return false;
  }

  // 计算两圆圆心之间的距离。
  double d = sqrt( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));

  // 计算各圆心到公共弦中点的距离。
  double a = (r1 - r2 + (d * d)) / (2 * d);
  double b = (r2 - r1 + (d * d)) / (2 * d);


  // 计算公共弦中点；完全重合等退化情况需要调用方结合算法限制理解。
  double p2X, p2Y;

  p2X = x1 + a * (x2 - x1) / d;
  p2Y = y1 + a * (y2 - y1) / d;

  // 计算公共弦一侧的交点。
  double h1 = sqrt((r1 * r1) - (a * a));

  p3X = p2X - h1 * (y2 - y1) / d;
  p3Y = p2Y + h1 * (x2 - x1) / d;


  // 计算公共弦另一侧的交点。
  double h2 = sqrt((r2 * r2) - (a * a));

  p4X = p2X + h2 * (y2 - y1) / d;
  p4Y = p2Y - h2 * (x2 - x1) / d;

  return true;

}

// 计算两圆重叠区域面积；使用扇形面积减三角形面积。算法参考：http://mathforum.org/library/drmath/view/54785.html。
double twoCirclesIntersectionArea(double x1, double y1, double r1,
                                  double x2, double y2, double r2)
{
  // 先计算两圆交点。
  double iX1, iY1, iX2, iY2;

  if(!twoCirclesIntersectionPoints(x1,y1,r1,x2,y2,r2,iX1,iY1,iX2,iY2))
  {
    return 0.0; // 未找到交点时，本实现返回零。
  }

  // 计算圆心之间的距离。
  double d = sqrt( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));

  // 根据圆心和交点计算两个扇形的圆心角。
  double cbd = 2 * acos((r2*r2 + d*d - r1*r1) / (r2 * d * 2));

  double cad = 2 * acos((r1*r1 + d*d - r2*r2) / (r1 * d * 2));


  // 每个弓形面积等于扇形面积减去三角形面积，再将两部分相加。

  double area = 0.5f*cbd*r2*r2 - 0.5f*r2*r2*sin(cbd) +
                0.5f*cad*r1*r1 - 0.5f*r1*r1*sin(cad);

  return area;
}

// 根据半径计算圆面积。
double circleArea(double radius)
{
  return geometryPi * radius * radius;
}


// 判断点到圆心的距离是否小于半径。
bool pointInCircle(Vector2D pos,
                   double    radius,
                   Vector2D p)
{
  double distFromCenterSquared = (p-pos).lengthSq();

  if (distFromCenterSquared < (radius*radius))
  {
    return true;
  }

  return false;
}

// 判断线段是否与给定圆相交。
bool lineSegmentCircleIntersection(Vector2D pointA,
                                   Vector2D pointB,
                                   Vector2D pointP,
                                   double    radius)
{
  // 计算圆心到线段的距离平方，并与半径平方比较。
  double distToLineSq = distToLineSegmentSq(pointA, pointB, pointP);

  if (distToLineSq < radius*radius)
  {
    return true;
  }

  else
  {
    return false;
  }

}

// 检查线段与圆是否相交；成功时通过引用返回距离线段起点最近的交点。
bool getLineSegmentCircleClosestIntersectionPoint(Vector2D pointA,
                                                  Vector2D pointB,
                                                  Vector2D pos,
                                                  double    radius,
                                                  Vector2D& intersectionPoint)
{
  Vector2D toBNorm = vec2DNormalize(pointB-pointA);

  // 将圆心转换到以线段起点为原点、线段方向为横轴的局部坐标。
  Vector2D localPos = pointToLocalSpace(pos, toBNorm, toBNorm.perp(), pointA);

  bool ipFound = false;

  // 圆整体位于线段起点之前或终点之后时，不可能相交。
  if ( (localPos.x+radius >= 0) &&
     ( (localPos.x-radius)*(localPos.x-radius) <= vec2DDistanceSq(pointB, pointA)) )
  {
     // 圆心到横轴的距离小于半径时，可能存在交点。
     if (fabs(localPos.y) < radius)
     {
        // 在局部坐标中求圆与横轴的交点，选择靠近起点的交点。
        double a = localPos.x;
        double b = localPos.y;

        double ip = a - sqrt(radius*radius - b*b);

        if (ip <= 0)
        {
          ip = a + sqrt(radius*radius - b*b);
        }

        ipFound = true;

        intersectionPoint = pointA+ toBNorm*ip;
     }
   }

  return ipFound;
}
