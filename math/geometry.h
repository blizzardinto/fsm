#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "utils.h"
#include "Vector2D.h"
#include "C2DMatrix.h"
#include "Transformations.h"

#include <math.h>
#include <vector>

const double pi = 3.14159;

enum span_type { plane_backside, plane_front, on_plane };

//given a plane and a ray this function determins how far along the ray 
//an interestion occurs. Returns negative if the ray is parallel
double DistanceToRayPlaneIntersection(Vector2D RayOrigin,
                                     Vector2D RayHeading,
                                     Vector2D PlanePoint,  //any point on the plane
                                     Vector2D PlaneNormal);

//------------------------- WhereIsPoint --------------------------------------
span_type WhereIsPoint(Vector2D point,
                       Vector2D PointOnPlane, //any point on the plane
                       Vector2D PlaneNormal);

//-------------------------- GetRayCircleIntersec -----------------------------
double GetRayCircleIntersect(Vector2D RayOrigin,
                             Vector2D RayHeading,
                             Vector2D CircleOrigin,
                             double  radius);

//----------------------------- DoRayCircleIntersect --------------------------
bool DoRayCircleIntersect(Vector2D RayOrigin,
                          Vector2D RayHeading,
                          Vector2D CircleOrigin,
                          double     radius);

//------------------------------------------------------------------------
//  Given a point P and a circle of radius R centered at C this function
//  determines the two points on the circle that intersect with the 
//  tangents from P to the circle. Returns false if P is within the circle.
//------------------------------------------------------------------------
bool GetTangentPoints (Vector2D C, double R, Vector2D P, Vector2D& T1, Vector2D& T2);

//------------------------- DistToLineSegment ----------------------------
//
//  given a line segment AB and a point P, this function calculates the 
//  perpendicular distance between them
//------------------------------------------------------------------------
double DistToLineSegment(Vector2D A,
                         Vector2D B,
                         Vector2D P);

//------------------------- DistToLineSegmentSq ----------------------------
//
//  as above, but avoiding sqrt
//------------------------------------------------------------------------
double DistToLineSegmentSq(Vector2D A,
                           Vector2D B,
                           Vector2D P);

//--------------------LineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an 
//	intersection occurs.
//
//----------------------------------------------------------------- 
bool LineIntersection2D(Vector2D A,
                        Vector2D B,
                        Vector2D C, 
                        Vector2D D);

//--------------------LineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an 
//	intersection occurs and sets dist to the distance the intersection
//  occurs along AB
//
//----------------------------------------------------------------- 
bool LineIntersection2D(Vector2D A,
                        Vector2D B,
                        Vector2D C, 
                        Vector2D D,
                        double &dist);

//-------------------- LineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an 
//	intersection occurs and sets dist to the distance the intersection
//  occurs along AB. Also sets the 2d vector point to the point of
//  intersection
//----------------------------------------------------------------- 
bool LineIntersection2D(Vector2D   A,
                        Vector2D   B,
                        Vector2D   C, 
                        Vector2D   D,
                        double&     dist,
                        Vector2D&  point);

//----------------------- ObjectIntersection2D ---------------------------
//
//  tests two polygons for intersection. *Does not check for enclosure*
//------------------------------------------------------------------------
bool ObjectIntersection2D(const std::vector<Vector2D>& object1,
                          const std::vector<Vector2D>& object2);

//----------------------- SegmentObjectIntersection2D --------------------
//
//  tests a line segment against a polygon for intersection
//  *Does not check for enclosure*
//------------------------------------------------------------------------
bool SegmentObjectIntersection2D(const Vector2D& A,
                                 const Vector2D& B,
                                 const std::vector<Vector2D>& object);

//----------------------------- TwoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
bool TwoCirclesOverlapped(double x1, double y1, double r1,
                          double x2, double y2, double r2);

//----------------------------- TwoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
bool TwoCirclesOverlapped(Vector2D c1, double r1,
                          Vector2D c2, double r2);

//--------------------------- TwoCirclesEnclosed ---------------------------
//
//  returns true if one circle encloses the other
//-------------------------------------------------------------------------
bool TwoCirclesEnclosed(double x1, double y1, double r1,
                        double x2, double y2, double r2);

//------------------------ TwoCirclesIntersectionPoints ------------------
//
//  Given two circles this function calculates the intersection points
//  of any overlap.
//
//  returns false if no overlap found
//------------------------------------------------------------------------ 
bool TwoCirclesIntersectionPoints(double x1, double y1, double r1,
                                  double x2, double y2, double r2,
                                  double &p3X, double &p3Y,
                                  double &p4X, double &p4Y);

//------------------------ TwoCirclesIntersectionArea --------------------
//
//  Tests to see if two circles overlap and if so calculates the area
//  defined by the union
//-----------------------------------------------------------------------
double TwoCirclesIntersectionArea(double x1, double y1, double r1,
                                  double x2, double y2, double r2);

//-------------------------------- CircleArea ---------------------------
//
//  given the radius, calculates the area of a circle
//-----------------------------------------------------------------------
double CircleArea(double radius);

//----------------------- PointInCircle ----------------------------------
//
//  returns true if the point p is within the radius of the given circle
//------------------------------------------------------------------------
bool PointInCircle(Vector2D Pos,
                   double    radius,
                   Vector2D p);

//--------------------- LineSegmentCircleIntersection ---------------------------
//
//  returns true if the line segment AB intersects with a circle at
//  position P with radius radius
//------------------------------------------------------------------------
bool LineSegmentCircleIntersection(Vector2D A,
                                   Vector2D B,
                                   Vector2D P,
                                   double    radius);

//------------------- GetLineSegmentCircleClosestIntersectionPoint ------------
//
//  given a line segment AB and a circle position and radius, this function
//  determines if there is an intersection and stores the position of the 
//  closest intersection in the reference IntersectionPoint
//
//  returns false if no intersection point is found
//-----------------------------------------------------------------------------
bool GetLineSegmentCircleClosestIntersectionPoint(Vector2D A,
                                                  Vector2D B,
                                                  Vector2D pos,
                                                  double    radius,
                                                  Vector2D& IntersectionPoint);

#endif
