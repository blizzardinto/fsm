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

//given a plane and a ray this function determins how far along the ray
//an interestion occurs. Returns negative if the ray is parallel
double distanceToRayPlaneIntersection(Vector2D rayOrigin,
                                     Vector2D rayHeading,
                                     Vector2D planePoint,  //any point on the plane
                                     Vector2D planeNormal);

//------------------------- whereIsPoint --------------------------------------
SpanType whereIsPoint(Vector2D point,
                       Vector2D pointOnPlane, //any point on the plane
                       Vector2D planeNormal);

//-------------------------- GetRayCircleIntersec -----------------------------
double getRayCircleIntersect(Vector2D rayOrigin,
                             Vector2D rayHeading,
                             Vector2D circleOrigin,
                             double  radius);

//----------------------------- doRayCircleIntersect --------------------------
bool doRayCircleIntersect(Vector2D rayOrigin,
                          Vector2D rayHeading,
                          Vector2D circleOrigin,
                          double     radius);

//------------------------------------------------------------------------
//  Given a point P and a circle of radius R centered at C this function
//  determines the two points on the circle that intersect with the
//  tangents from P to the circle. Returns false if P is within the circle.
//------------------------------------------------------------------------
bool getTangentPoints (Vector2D c, double radiusValue, Vector2D pointP, Vector2D& t1, Vector2D& t2);

//------------------------- distToLineSegment ----------------------------
//
//  given a line segment AB and a point P, this function calculates the
//  perpendicular distance between them
//------------------------------------------------------------------------
double distToLineSegment(Vector2D pointA,
                         Vector2D pointB,
                         Vector2D pointP);

//------------------------- distToLineSegmentSq ----------------------------
//
//  as above, but avoiding sqrt
//------------------------------------------------------------------------
double distToLineSegmentSq(Vector2D pointA,
                           Vector2D pointB,
                           Vector2D pointP);

//--------------------lineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an
//	intersection occurs.
//
//-----------------------------------------------------------------
bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD);

//--------------------lineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an
//	intersection occurs and sets dist to the distance the intersection
//  occurs along AB
//
//-----------------------------------------------------------------
bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD,
                        double &dist);

//-------------------- lineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an
//	intersection occurs and sets dist to the distance the intersection
//  occurs along AB. Also sets the 2d vector point to the point of
//  intersection
//-----------------------------------------------------------------
bool lineIntersection2D(Vector2D   pointA,
                        Vector2D   pointB,
                        Vector2D   c,
                        Vector2D   pointD,
                        double&     dist,
                        Vector2D&  point);

//----------------------- objectIntersection2D ---------------------------
//
//  tests two polygons for intersection. *Does not check for enclosure*
//------------------------------------------------------------------------
bool objectIntersection2D(const std::vector<Vector2D>& object1,
                          const std::vector<Vector2D>& object2);

//----------------------- segmentObjectIntersection2D --------------------
//
//  tests a line segment against a polygon for intersection
//  *Does not check for enclosure*
//------------------------------------------------------------------------
bool segmentObjectIntersection2D(const Vector2D& pointA,
                                 const Vector2D& pointB,
                                 const std::vector<Vector2D>& object);

//----------------------------- twoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
bool twoCirclesOverlapped(double x1, double y1, double r1,
                          double x2, double y2, double r2);

//----------------------------- twoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
bool twoCirclesOverlapped(Vector2D c1, double r1,
                          Vector2D c2, double r2);

//--------------------------- twoCirclesEnclosed ---------------------------
//
//  returns true if one circle encloses the other
//-------------------------------------------------------------------------
bool twoCirclesEnclosed(double x1, double y1, double r1,
                        double x2, double y2, double r2);

//------------------------ twoCirclesIntersectionPoints ------------------
//
//  Given two circles this function calculates the intersection points
//  of any overlap.
//
//  returns false if no overlap found
//------------------------------------------------------------------------
bool twoCirclesIntersectionPoints(double x1, double y1, double r1,
                                  double x2, double y2, double r2,
                                  double &p3X, double &p3Y,
                                  double &p4X, double &p4Y);

//------------------------ twoCirclesIntersectionArea --------------------
//
//  Tests to see if two circles overlap and if so calculates the area
//  defined by the union
//-----------------------------------------------------------------------
double twoCirclesIntersectionArea(double x1, double y1, double r1,
                                  double x2, double y2, double r2);

//-------------------------------- circleArea ---------------------------
//
//  given the radius, calculates the area of a circle
//-----------------------------------------------------------------------
double circleArea(double radius);

//----------------------- pointInCircle ----------------------------------
//
//  returns true if the point p is within the radius of the given circle
//------------------------------------------------------------------------
bool pointInCircle(Vector2D pos,
                   double    radius,
                   Vector2D p);

//--------------------- lineSegmentCircleIntersection ---------------------------
//
//  returns true if the line segment AB intersects with a circle at
//  position P with radius radius
//------------------------------------------------------------------------
bool lineSegmentCircleIntersection(Vector2D pointA,
                                   Vector2D pointB,
                                   Vector2D pointP,
                                   double    radius);

//------------------- getLineSegmentCircleClosestIntersectionPoint ------------
//
//  given a line segment AB and a circle position and radius, this function
//  determines if there is an intersection and stores the position of the
//  closest intersection in the reference intersectionPoint
//
//  returns false if no intersection point is found
//-----------------------------------------------------------------------------
bool getLineSegmentCircleClosestIntersectionPoint(Vector2D pointA,
                                                  Vector2D pointB,
                                                  Vector2D pos,
                                                  double    radius,
                                                  Vector2D& intersectionPoint);

#endif
