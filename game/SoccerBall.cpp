#include "SoccerBall.h"
#include "geometry.h"
#include "DebugConsole.h"
#include "Cgdi.h"
#include "ParamLoader.h"
#include "Wall2D.h"


//----------------------------- addNoiseToKick --------------------------------
//
//  this can be used to vary the accuracy of a player's kick. Just call it
//  prior to kicking the ball using the ball's position and the ball target as
//  parameters.
//-----------------------------------------------------------------------------
Vector2D addNoiseToKick(Vector2D ballPos, Vector2D ballTarget)
{

  double displacement = (pi - pi*prm.playerKickingAccuracy) * randomClamped();

  Vector2D toTarget = ballTarget - ballPos;

  vec2DRotateAroundOrigin(toTarget, displacement);

  return toTarget + ballPos;
}



//-------------------------- kick ----------------------------------------
//
//  applys a force to the ball in the direction of heading. Truncates
//  the new velocity to make sure it doesn't exceed the max allowable.
//------------------------------------------------------------------------
void SoccerBall::kick(Vector2D direction, double force)
{
  //ensure direction is normalized
  direction.normalize();

  //calculate the acceleration
  Vector2D acceleration = (direction * force) / mMass;

  //update the velocity
  mVelocity = acceleration;
}

//----------------------------- update -----------------------------------
//
//  updates the ball physics, tests for any collisions and adjusts
//  the ball's velocity accordingly
//------------------------------------------------------------------------
void SoccerBall::update()
{
  //keep a record of the old position so the goal::scored method
  //can utilize it for goal testing
  mOldPos = mPosition;

      //Test for collisions
    testCollisionWithWalls(mPitchBoundary);

  //Simulate prm.friction. Make sure the speed is positive
  //first though
  if (mVelocity.lengthSq() > prm.friction * prm.friction)
  {
    mVelocity += vec2DNormalize(mVelocity) * prm.friction;

    mPosition += mVelocity;



    //update heading
    mHeading = vec2DNormalize(mVelocity);
  }
}

//---------------------- timeToCoverDistance -----------------------------
//
//  Given a force and a distance to cover given by two vectors, this
//  method calculates how long it will take the ball to travel between
//  the two points
//------------------------------------------------------------------------
double SoccerBall::timeToCoverDistance(Vector2D pointA,
                                      Vector2D pointB,
                                      double force)const
{
  //this will be the velocity of the ball in the next time step *if*
  //the player was to make the pass.
  double speed = force / mMass;

  //calculate the velocity at B using the equation
  //
  //  v^2 = u^2 + 2as
  //

  //first calculate s (the distance between the two positions)
  double distanceToCover =  vec2DDistance(pointA, pointB);

  double term = speed*speed + 2.0*distanceToCover*prm.friction;

  //if  (u^2 + 2as) is negative it means the ball cannot reach point B.
  if (term <= 0.0) return -1.0;

  double v = sqrt(term);

  //it IS possible for the ball to reach B and we know its speed when it
  //gets there, so now it's easy to calculate the time using the equation
  //
  //    t = v-u
  //        ---
  //         a
  //
  return (v-speed)/prm.friction;
}

//--------------------- futurePosition -----------------------------------
//
//  given a time this method returns the ball position at that time in the
//  future
//------------------------------------------------------------------------
Vector2D SoccerBall::futurePosition(double time)const
{
  //using the equation s = ut + 1/2at^2, where s = distance, a = friction
  //u=start velocity

  //calculate the ut term, which is a vector
  Vector2D ut = mVelocity * time;

  //calculate the 1/2at^2 term, which is scalar
  double halfAccelerationTimeSquared = 0.5 * prm.friction * time * time;

  //turn the scalar quantity into a vector by multiplying the value with
  //the normalized velocity vector (because that gives the direction)
  Vector2D scalarToVector = halfAccelerationTimeSquared * vec2DNormalize(mVelocity);

  //the predicted position is the balls position plus these two terms
  return pos() + ut + scalarToVector;
}


//----------------------------- render -----------------------------------
//
//  Renders the ball
//------------------------------------------------------------------------
void SoccerBall::render()
{
  gdi->blackBrush();

  gdi->circle(mPosition, mBoundingRadius);

  /*
  gdi->greenBrush();
  for (int i=0; i<IPPoints.size(); ++i)
  {
    gdi->circle(IPPoints[i], 3);
  }
  */
}


//----------------------- testCollisionWithWalls -------------------------
//
void SoccerBall::testCollisionWithWalls(const std::vector<Wall2D>& walls)
{
  //test ball against each wall, find out which is closest
  int idxClosest = -1;

  Vector2D velNormal = vec2DNormalize(mVelocity);

  Vector2D intersectionPoint, collisionPoint;

  double distToIntersection = maxFloat;

  //iterate through each wall and calculate if the ball intersects.
  //If it does then store the index into the closest intersecting wall
  for (unsigned int w=0; w<walls.size(); ++w)
  {
    //assuming a collision if the ball continued on its current heading
    //calculate the point on the ball that would hit the wall. This is
    //simply the wall's normal(inversed) multiplied by the ball's radius
    //and added to the balls center (its position)
    Vector2D thisCollisionPoint = pos() - (walls[w].normal() * boundingRadius());

    //calculate exactly where the collision point will hit the plane
    if (whereIsPoint(thisCollisionPoint,
                     walls[w].from(),
                     walls[w].normal()) == planeBackside)
    {
      double distToWall = distanceToRayPlaneIntersection(thisCollisionPoint,
                                                         walls[w].normal(),
                                                         walls[w].from(),
                                                         walls[w].normal());

      intersectionPoint = thisCollisionPoint + (distToWall * walls[w].normal());

    }

    else
    {
      double distToWall = distanceToRayPlaneIntersection(thisCollisionPoint,
                                                         velNormal,
                                                         walls[w].from(),
                                                         walls[w].normal());

      intersectionPoint = thisCollisionPoint + (distToWall * velNormal);
    }

    //check to make sure the intersection point is actually on the line
    //segment
    bool onLineSegment = false;

    if (lineIntersection2D(walls[w].from(),
                           walls[w].to(),
                           thisCollisionPoint - walls[w].normal()*20.0,
                           thisCollisionPoint + walls[w].normal()*20.0))
    {

      onLineSegment = true;
    }


                                                                          //Note, there is no test for collision with the end of a line segment

    //now check to see if the collision point is within range of the
    //velocity vector. [work in distance squared to avoid sqrt] and if it
    //is the closest hit found so far.
    //If it is that means the ball will collide with the wall sometime
    //between this time step and the next one.
    double distSq = vec2DDistanceSq(thisCollisionPoint, intersectionPoint);

    if ((distSq <= mVelocity.lengthSq()) && (distSq < distToIntersection) && onLineSegment)
    {
      distToIntersection = distSq;
      idxClosest = w;
      collisionPoint = intersectionPoint;
    }
  }//next wall


  //to prevent having to calculate the exact time of collision we
  //can just check if the velocity is opposite to the wall normal
  //before reflecting it. This prevents the case where there is overshoot
  //and the ball gets reflected back over the line before it has completely
  //reentered the playing area.
  if ( (idxClosest >= 0 ) && velNormal.dot(walls[idxClosest].normal()) < 0)
  {
    mVelocity.reflect(walls[idxClosest].normal());
  }
}

//----------------------- PlaceAtLocation -------------------------------------
//
//  positions the ball at the desired location and sets the ball's velocity to
//  zero
//-----------------------------------------------------------------------------
void SoccerBall::placeAtPosition(Vector2D newPos)
{
  mPosition = newPos;

  mOldPos = mPosition;

  mVelocity.zero();
}

