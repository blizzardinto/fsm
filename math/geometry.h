/*
 * 阅读提示：几何算法集合，提供线段、圆、射线和多边形的相交与距离判断。
 * 这些计算不依赖球队或窗口，适合用普通函数表达；面向对象并不要求把所有算法都包装成类。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "utils.h"
#include "Vector2D.h"
#include "C2DMatrix.h"
#include "Transformations.h"

#include <math.h>
#include <vector>

const double geometryPi = 3.14159;

enum SpanType { planeBackside, planeFront, onPlane };

// 求射线与平面交点沿射线方向的距离；平行时返回负值。
double distanceToRayPlaneIntersection(Vector2D rayOrigin,
                                     Vector2D rayHeading,
                                     Vector2D planePoint,  // 平面上的任意一个点。
                                     Vector2D planeNormal);

// 判断点位于平面的哪一侧。
SpanType whereIsPoint(Vector2D point,
                       Vector2D pointOnPlane, // 平面上的任意一个点。
                       Vector2D planeNormal);

// 求射线与圆的第一个交点距离。
double getRayCircleIntersect(Vector2D rayOrigin,
                             Vector2D rayHeading,
                             Vector2D circleOrigin,
                             double  radius);

// 注意实现返回判别式小于零，即无实交点；其布尔语义与函数名称相反。
bool doRayCircleIntersect(Vector2D rayOrigin,
                          Vector2D rayHeading,
                          Vector2D circleOrigin,
                          double     radius);

// 从圆外点向圆作切线并返回两个切点；圆内或圆上的点返回假。
bool getTangentPoints (Vector2D c, double radiusValue, Vector2D pointP, Vector2D& t1, Vector2D& t2);

// 求点到线段的最短距离，必要时使用端点距离。
double distToLineSegment(Vector2D pointA,
                         Vector2D pointB,
                         Vector2D pointP);

// 求点到线段的距离平方，避免开平方。
double distToLineSegmentSq(Vector2D pointA,
                           Vector2D pointB,
                           Vector2D pointP);

// 判断两条二维线段是否相交。
bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD);

// 判断线段相交，并返回沿第一条线段到交点的距离。
bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD,
                        double &dist);

// 判断线段相交，并返回交点距离及坐标。
bool lineIntersection2D(Vector2D   pointA,
                        Vector2D   pointB,
                        Vector2D   c,
                        Vector2D   pointD,
                        double&     dist,
                        Vector2D&  point);

// 检查多边形边之间是否相交，不检测完全包含关系。
bool objectIntersection2D(const std::vector<Vector2D>& object1,
                          const std::vector<Vector2D>& object2);

// 检查线段与多边形边界是否相交，不检测完全包含关系。
bool segmentObjectIntersection2D(const Vector2D& pointA,
                                 const Vector2D& pointB,
                                 const std::vector<Vector2D>& object);

// 判断两个圆是否重叠。
bool twoCirclesOverlapped(double x1, double y1, double r1,
                          double x2, double y2, double r2);

// 通过圆心向量判断两个圆是否重叠。
bool twoCirclesOverlapped(Vector2D c1, double r1,
                          Vector2D c2, double r2);

// 判断一个圆是否完全包含另一个圆。
bool twoCirclesEnclosed(double x1, double y1, double r1,
                        double x2, double y2, double r2);

// 计算两圆交点，无符合条件的重叠时返回假。
bool twoCirclesIntersectionPoints(double x1, double y1, double r1,
                                  double x2, double y2, double r2,
                                  double &p3X, double &p3Y,
                                  double &p4X, double &p4Y);

// 计算两圆重叠部分的面积。
double twoCirclesIntersectionArea(double x1, double y1, double r1,
                                  double x2, double y2, double r2);

// 根据半径计算圆面积。
double circleArea(double radius);

// 判断点是否位于圆内部。
bool pointInCircle(Vector2D pos,
                   double    radius,
                   Vector2D p);

// 判断线段与圆是否相交。
bool lineSegmentCircleIntersection(Vector2D pointA,
                                   Vector2D pointB,
                                   Vector2D pointP,
                                   double    radius);

// 查找线段与圆的最近交点，并通过引用返回坐标。
bool getLineSegmentCircleClosestIntersectionPoint(Vector2D pointA,
                                                  Vector2D pointB,
                                                  Vector2D pos,
                                                  double    radius,
                                                  Vector2D& intersectionPoint);

#endif
