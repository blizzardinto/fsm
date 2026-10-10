#include "geometry.h"

//given a plane and a ray this function determins how far along the ray
//an interestion occurs. Returns negative if the ray is parallel
double distanceToRayPlaneIntersection(Vector2D rayOrigin,
                                     Vector2D rayHeading,
                                     Vector2D planePoint,  //any point on the plane
                                     Vector2D planeNormal)
{

  double d     = - planeNormal.dot(planePoint);
  double numer = planeNormal.dot(rayOrigin) + d;
  double denom = planeNormal.dot(rayHeading);

  // normal is parallel to vector
  if ((denom < 0.000001) && (denom > -0.000001))
  {
   return (-1.0);
  }

  return -(numer / denom);
}

//------------------------- whereIsPoint --------------------------------------
SpanType whereIsPoint(Vector2D point,
                       Vector2D pointOnPlane, //any point on the plane
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


//-------------------------- GetRayCircleIntersec -----------------------------
double getRayCircleIntersect(Vector2D rayOrigin,
                             Vector2D rayHeading,
                             Vector2D circleOrigin,
                             double  radius)
{

   Vector2D toCircle = circleOrigin-rayOrigin;
   double length      = toCircle.length();
   double v           = toCircle.dot(rayHeading);
   double d           = radius*radius - (length*length - v*v);

   // If there was no intersection, return -1
   if (d < 0.0) return (-1.0);

   // Return the distance to the [first] intersecting point
   return (v - sqrt(d));
}

//----------------------------- doRayCircleIntersect --------------------------
bool doRayCircleIntersect(Vector2D rayOrigin,
                          Vector2D rayHeading,
                          Vector2D circleOrigin,
                          double     radius)
{

   Vector2D toCircle = circleOrigin-rayOrigin;
   double length      = toCircle.length();
   double v           = toCircle.dot(rayHeading);
   double d           = radius*radius - (length*length - v*v);

   // If there was no intersection, return -1
   return (d < 0.0);
}


//------------------------------------------------------------------------
//  Given a point P and a circle of radius R centered at C this function
//  determines the two points on the circle that intersect with the
//  tangents from P to the circle. Returns false if P is within the circle.
//
//  thanks to Dave Eberly for this one.
//------------------------------------------------------------------------
bool getTangentPoints (Vector2D c, double radiusValue, Vector2D pointP, Vector2D& t1, Vector2D& t2)
{
  Vector2D pmC = pointP - c;
  double sqrLen = pmC.lengthSq();
  double rSqr = radiusValue*radiusValue;
  if ( sqrLen <= rSqr )
  {
      // P is inside or on the circle
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




//------------------------- distToLineSegment ----------------------------
//
//  given a line segment AB and a point P, this function calculates the
//  perpendicular distance between them
//------------------------------------------------------------------------
double distToLineSegment(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D pointP)
{
  //if the angle is obtuse between PA and AB is obtuse then the closest
  //vertex must be A
  double dotA = (pointP.x - pointA.x)*(pointB.x - pointA.x) + (pointP.y - pointA.y)*(pointB.y - pointA.y);

  if (dotA <= 0) return vec2DDistance(pointA, pointP);

  //if the angle is obtuse between PB and AB is obtuse then the closest
  //vertex must be B
  double dotB = (pointP.x - pointB.x)*(pointA.x - pointB.x) + (pointP.y - pointB.y)*(pointA.y - pointB.y);

  if (dotB <= 0) return vec2DDistance(pointB, pointP);

  //calculate the point along AB that is the closest to P
  Vector2D point = pointA + ((pointB - pointA) * dotA)/(dotA + dotB);

  //calculate the distance P-point
  return vec2DDistance(pointP,point);
}

//------------------------- distToLineSegmentSq ----------------------------
//
//  as above, but avoiding sqrt
//------------------------------------------------------------------------
double distToLineSegmentSq(Vector2D pointA,
                          Vector2D pointB,
                          Vector2D pointP)
{
  //if the angle is obtuse between PA and AB is obtuse then the closest
  //vertex must be A
  double dotA = (pointP.x - pointA.x)*(pointB.x - pointA.x) + (pointP.y - pointA.y)*(pointB.y - pointA.y);

  if (dotA <= 0) return vec2DDistanceSq(pointA, pointP);

  //if the angle is obtuse between PB and AB is obtuse then the closest
  //vertex must be B
  double dotB = (pointP.x - pointB.x)*(pointA.x - pointB.x) + (pointP.y - pointB.y)*(pointA.y - pointB.y);

  if (dotB <= 0) return vec2DDistanceSq(pointB, pointP);

  //calculate the point along AB that is the closest to P
  Vector2D point = pointA + ((pointB - pointA) * dotA)/(dotA + dotB);

  //calculate the distance P-point
  return vec2DDistanceSq(pointP,point);
}


//--------------------lineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an
//	intersection occurs.
//
//-----------------------------------------------------------------

bool lineIntersection2D(Vector2D pointA,
                        Vector2D pointB,
                        Vector2D c,
                        Vector2D pointD)
{
  double rTop = (pointA.y-c.y)*(pointD.x-c.x)-(pointA.x-c.x)*(pointD.y-c.y);
  double sTop = (pointA.y-c.y)*(pointB.x-pointA.x)-(pointA.x-c.x)*(pointB.y-pointA.y);

  double bot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);

  if (bot == 0)//parallel
  {
    return false;
  }

  double invBot = 1.0/bot;
  double r = rTop * invBot;
  double s = sTop * invBot;

  if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
  {
    //lines intersect
    return true;
  }

  //lines do not intersect
  return false;
}

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
                        double &dist)
{

  double rTop = (pointA.y-c.y)*(pointD.x-c.x)-(pointA.x-c.x)*(pointD.y-c.y);
  double sTop = (pointA.y-c.y)*(pointB.x-pointA.x)-(pointA.x-c.x)*(pointB.y-pointA.y);

  double bot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);


  if (bot == 0)//parallel
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
                        Vector2D&  point)
{

  double rTop = (pointA.y-c.y)*(pointD.x-c.x)-(pointA.x-c.x)*(pointD.y-c.y);
  double rBot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);

  double sTop = (pointA.y-c.y)*(pointB.x-pointA.x)-(pointA.x-c.x)*(pointB.y-pointA.y);
  double sBot = (pointB.x-pointA.x)*(pointD.y-c.y)-(pointB.y-pointA.y)*(pointD.x-c.x);

  if ( (rBot == 0) || (sBot == 0))
  {
    //lines are parallel
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

//----------------------- objectIntersection2D ---------------------------
//
//  tests two polygons for intersection. *Does not check for enclosure*
//------------------------------------------------------------------------
bool objectIntersection2D(const std::vector<Vector2D>& object1,
                          const std::vector<Vector2D>& object2)
{
  //test each line segment of object1 against each segment of object2
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

//----------------------- segmentObjectIntersection2D --------------------
//
//  tests a line segment against a polygon for intersection
//  *Does not check for enclosure*
//------------------------------------------------------------------------
bool segmentObjectIntersection2D(const Vector2D& pointA,
                                 const Vector2D& pointB,
                                 const std::vector<Vector2D>& object)
{
  //test AB against each segment of object
  for (unsigned int r=0; r<object.size()-1; ++r)
  {
    if (lineIntersection2D(pointA, pointB, object[r], object[r+1]))
    {
      return true;
    }
  }

  return false;
}


//----------------------------- twoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
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

//----------------------------- twoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
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

//--------------------------- twoCirclesEnclosed ---------------------------
//
//  returns true if one circle encloses the other
//-------------------------------------------------------------------------
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

//------------------------ twoCirclesIntersectionPoints ------------------
//
//  Given two circles this function calculates the intersection points
//  of any overlap.
//
//  returns false if no overlap found
//
// see http://astronomy.swin.edu.au/~pbourke/geometry/2circle/
//------------------------------------------------------------------------
bool twoCirclesIntersectionPoints(double x1, double y1, double r1,
                                  double x2, double y2, double r2,
                                  double &p3X, double &p3Y,
                                  double &p4X, double &p4Y)
{
  //first check to see if they overlap
  if (!twoCirclesOverlapped(x1,y1,r1,x2,y2,r2))
  {
    return false;
  }

  //calculate the distance between the circle centers
  double d = sqrt( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));

  //Now calculate the distance from the center of each circle to the center
  //of the line which connects the intersection points.
  double a = (r1 - r2 + (d * d)) / (2 * d);
  double b = (r2 - r1 + (d * d)) / (2 * d);


  //MAYBE A TEST FOR EXACT OVERLAP?

  //calculate the point P2 which is the center of the line which
  //connects the intersection points
  double p2X, p2Y;

  p2X = x1 + a * (x2 - x1) / d;
  p2Y = y1 + a * (y2 - y1) / d;

  //calculate first point
  double h1 = sqrt((r1 * r1) - (a * a));

  p3X = p2X - h1 * (y2 - y1) / d;
  p3Y = p2Y + h1 * (x2 - x1) / d;


  //calculate second point
  double h2 = sqrt((r2 * r2) - (a * a));

  p4X = p2X + h2 * (y2 - y1) / d;
  p4Y = p2Y - h2 * (x2 - x1) / d;

  return true;

}

//------------------------ twoCirclesIntersectionArea --------------------
//
//  Tests to see if two circles overlap and if so calculates the area
//  defined by the union
//
// see http://mathforum.org/library/drmath/view/54785.html
//-----------------------------------------------------------------------
double twoCirclesIntersectionArea(double x1, double y1, double r1,
                                  double x2, double y2, double r2)
{
  //first calculate the intersection points
  double iX1, iY1, iX2, iY2;

  if(!twoCirclesIntersectionPoints(x1,y1,r1,x2,y2,r2,iX1,iY1,iX2,iY2))
  {
    return 0.0; //no overlap
  }

  //calculate the distance between the circle centers
  double d = sqrt( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));

  //find the angles given that A and B are the two circle centers
  //and C and D are the intersection points
  double cbd = 2 * acos((r2*r2 + d*d - r1*r1) / (r2 * d * 2));

  double cad = 2 * acos((r1*r1 + d*d - r2*r2) / (r1 * d * 2));


  //Then we find the segment of each of the circles cut off by the
  //chord CD, by taking the area of the sector of the circle BCD and
  //subtracting the area of triangle BCD. Similarly we find the area
  //of the sector ACD and subtract the area of triangle ACD.

  double area = 0.5f*cbd*r2*r2 - 0.5f*r2*r2*sin(cbd) +
                0.5f*cad*r1*r1 - 0.5f*r1*r1*sin(cad);

  return area;
}

//-------------------------------- circleArea ---------------------------
//
//  given the radius, calculates the area of a circle
//-----------------------------------------------------------------------
double circleArea(double radius)
{
  return geometryPi * radius * radius;
}


//----------------------- pointInCircle ----------------------------------
//
//  returns true if the point p is within the radius of the given circle
//------------------------------------------------------------------------
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

//--------------------- lineSegmentCircleIntersection ---------------------------
//
//  returns true if the line segemnt AB intersects with a circle at
//  position P with radius radius
//------------------------------------------------------------------------
bool lineSegmentCircleIntersection(Vector2D pointA,
                                   Vector2D pointB,
                                   Vector2D pointP,
                                   double    radius)
{
  //first determine the distance from the center of the circle to
  //the line segment (working in distance squared space)
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
                                                  Vector2D& intersectionPoint)
{
  Vector2D toBNorm = vec2DNormalize(pointB-pointA);

  //move the circle into the local space defined by the vector B-A with origin
  //at A
  Vector2D localPos = pointToLocalSpace(pos, toBNorm, toBNorm.perp(), pointA);

  bool ipFound = false;

  //if the local position + the radius is negative then the circle lays behind
  //point A so there is no intersection possible. If the local x pos minus the
  //radius is greater than length A-B then the circle cannot intersect the
  //line segment
  if ( (localPos.x+radius >= 0) &&
     ( (localPos.x-radius)*(localPos.x-radius) <= vec2DDistanceSq(pointB, pointA)) )
  {
     //if the distance from the x axis to the object's position is less
     //than its radius then there is a potential intersection.
     if (fabs(localPos.y) < radius)
     {
        //now to do a line/circle intersection test. The center of the
        //circle is represented by A, B. The intersection points are
        //given by the formulae x = A +/-sqrt(r^2-B^2), y=0. We only
        //need to look at the smallest positive value of x.
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
