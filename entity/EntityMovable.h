#ifndef MOVING_ENTITY
#define MOVING_ENTITY
//------------------------------------------------------------------------
//
//  name:   EntityMovable.h
//
//  Desc:   A base class defining an entity that moves. The entity has
//          a local coordinate system and members for defining its
//          mass and velocity.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------

#include <cassert>

#include "Vector2D.h"
#include "EntityBase.h"



class EntityMovable : public EntityBase
{
protected:

  Vector2D    mVelocity;

  //a normalized vector pointing in the direction the entity is heading.
  Vector2D    mHeading;

  //a vector perpendicular to the heading vector
  Vector2D    mSide;

  double      mMass;

  //the maximum speed this entity may travel at.
  double      mMaxSpeed;

  //the maximum force this entity can produce to power itself
  //(think rockets and thrust)
  double      mMaxForce;

  //the maximum rate (radians per second)this vehicle can rotate
  double      mMaxTurnRate;

public:


  EntityMovable(Vector2D position,
               double   radius,
               Vector2D velocity,
               double   maxSpeed,
               Vector2D heading,
               double   mass,
               Vector2D scale,
               double   turnRate,
               double   maxForce):EntityBase(EntityBase::getNextValidId()),
                                  mHeading(heading),
                                  mVelocity(velocity),
                                  mMass(mass),
                                  mSide(mHeading.perp()),
                                  mMaxSpeed(maxSpeed),
                                  mMaxTurnRate(turnRate),
                                  mMaxForce(maxForce)
  {
    mPosition = position;
    mBoundingRadius = radius;
    mScale = scale;
  }


  virtual ~EntityMovable(){}

  //accessors
  Vector2D  velocity()const{return mVelocity;}
  void      setVelocity(const Vector2D& newVel){mVelocity = newVel;}

  double    mass()const{return mMass;}

  Vector2D  side()const{return mSide;}

  double    maxSpeed()const{return mMaxSpeed;}
  void      setMaxSpeed(double newSpeed){mMaxSpeed = newSpeed;}

  double    maxForce()const{return mMaxForce;}
  void      setMaxForce(double mf){mMaxForce = mf;}

  bool      isSpeedMaxedOut()const{return mMaxSpeed*mMaxSpeed >= mVelocity.lengthSq();}
  double    speed()const{return mVelocity.length();}
  double    speedSq()const{return mVelocity.lengthSq();}

  Vector2D  heading()const{return mHeading;}
  void      setHeading(Vector2D newHeading);
  bool      rotateHeadingToFacePosition(Vector2D target);

  double    maxTurnRate()const{return mMaxTurnRate;}
  void      setMaxTurnRate(double val){mMaxTurnRate = val;}

};


//--------------------------- rotateHeadingToFacePosition ---------------------
//
//  given a target position, this method rotates the entity's heading and
//  side vectors by an amount not greater than mMaxTurnRate until it
//  directly faces the target.
//
//  returns true when the heading is facing in the desired direction
//-----------------------------------------------------------------------------
inline bool EntityMovable::rotateHeadingToFacePosition(Vector2D target)
{
  Vector2D toTarget = vec2DNormalize(target - mPosition);

  double dot = mHeading.dot(toTarget);

  //some compilers lose acurracy so the value is clamped to ensure it
  //remains valid for the acos
  clamp(dot, -1, 1);

  //first determine the angle between the heading vector and the target
  double angle = acos(dot);

  //return true if the player is facing the target
  if (angle < 0.00001) return true;

  //clamp the amount to turn to the max turn rate
  if (angle > mMaxTurnRate) angle = mMaxTurnRate;

  //The next few lines use a rotation matrix to rotate the player's heading
  //vector accordingly
  C2DMatrix rotationMatrix;

  //notice how the direction of rotation has to be determined when creating
  //the rotation matrix
  rotationMatrix.rotate(angle * mHeading.sign(toTarget));
  rotationMatrix.transformVector2Ds(mHeading);
  rotationMatrix.transformVector2Ds(mVelocity);

  //finally recreate mSide
  mSide = mHeading.perp();

  return false;
}


//------------------------- setHeading ----------------------------------------
//
//  first checks that the given heading is not a vector of zero length. If the
//  new heading is valid this fumction sets the entity's heading and side
//  vectors accordingly
//-----------------------------------------------------------------------------
inline void EntityMovable::setHeading(Vector2D newHeading)
{
  assert( (newHeading.lengthSq() - 1.0) < 0.00001);

  mHeading = newHeading;

  //the side vector must always be perpendicular to the heading
  mSide = mHeading.perp();
}




#endif