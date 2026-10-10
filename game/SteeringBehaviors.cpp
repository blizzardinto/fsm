#include "SteeringBehaviors.h"
#include "EntityPlayer.h"
#include "Transformations.h"
#include "utils.h"
#include "SoccerTeam.h"
#include "autolist.h"
#include "ParamLoader.h"
#include "SoccerBall.h"
#include <algorithm>


using std::string;
using std::vector;

//------------------------- ctor -----------------------------------------
//
//------------------------------------------------------------------------
SteeringBehaviors::SteeringBehaviors(EntityPlayer*  agent,
                                     SoccerPitch* world,
                                     SoccerBall*  ball):

             mPlayer(agent),
             mFlags(0),
             mMultSeparation(prm.separationCoefficient),
             mTagged(false),
             mViewDistance(prm.viewDistance),
             mBall(ball),
             mInterposeDist(0.0),
             mAntenna(5,Vector2D())
{
}

//--------------------- accumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool SteeringBehaviors::accumulateForce(Vector2D &sf, Vector2D forceToAdd)
{
  //first calculate how much steering force we have left to use
  double magnitudeSoFar = sf.length();

  double magnitudeRemaining = mPlayer->maxForce() - magnitudeSoFar;

  //return false if there is no more force left to use
  if (magnitudeRemaining <= 0.0) return false;

  //calculate the magnitude of the force we want to add
  double magnitudeToAdd = forceToAdd.length();

  //now calculate how much of the force we can really add
  if (magnitudeToAdd > magnitudeRemaining)
  {
    magnitudeToAdd = magnitudeRemaining;
  }

  //add it to the steering force
  sf += (vec2DNormalize(forceToAdd) * magnitudeToAdd);

  return true;
}

//---------------------- calculate ---------------------------------------
//
//  calculates the overall steering force based on the currently active
//  steering behaviors.
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::calculate()
{
  //reset the force
  mSteeringForce.zero();

  //this will hold the value of each individual steering force
  mSteeringForce = sumForces();

  //make sure the force doesn't exceed the vehicles maximum allowable
  mSteeringForce.truncate(mPlayer->maxForce());

  return mSteeringForce;
}

//-------------------------- sumForces -----------------------------------
//
//  this method calls each active steering behavior and acumulates their
//  forces until the max steering force magnitude is reached at which
//  time the function returns the steering force accumulated to that
//  point
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::sumForces()
{
   Vector2D force;

  //the soccer players must always tag their neighbors
   findNeighbours();

  if (on(separationBehavior))
  {
    force += separation() * mMultSeparation;

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(seekBehavior))
  {
    force += seek(mTarget);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(arriveBehavior))
  {
    force += arrive(mTarget, fast);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(pursuitBehavior))
  {
    force += pursuit(mBall);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(interposeBehavior))
  {
    force += interpose(mBall, mTarget, mInterposeDist);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  return mSteeringForce;
}

//------------------------- forwardComponent -----------------------------
//
//  calculates the forward component of the steering force
//------------------------------------------------------------------------
double SteeringBehaviors::forwardComponent()
{
  return mPlayer->heading().dot(mSteeringForce);
}

//--------------------------- sideComponent ------------------------------
//
//  //  calculates the side component of the steering force
//------------------------------------------------------------------------
double SteeringBehaviors::sideComponent()
{
  return mPlayer->side().dot(mSteeringForce) * mPlayer->maxTurnRate();
}


//------------------------------- seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  allign the agent with the target and move the agent in the desired
//  direction
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::seek(Vector2D target)
{

  Vector2D desiredVelocity = vec2DNormalize(target - mPlayer->pos())
                            * mPlayer->maxSpeed();

  return (desiredVelocity - mPlayer->velocity());
}


//--------------------------- arrive -------------------------------------
//
//  This behavior is similar to seekBehavior but it attempts to arriveBehavior at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::arrive(Vector2D    target,
                                   Deceleration deceleration)
{
  Vector2D toTarget = target - mPlayer->pos();

  //calculate the distance to the target
  double dist = toTarget.length();

  if (dist > 0)
  {
    //because Deceleration is enumerated as an int, this value is required
    //to provide fine tweaking of the deceleration..
    const double decelerationTweaker = 0.3;

    //calculate the speed required to reach the target given the desired
    //deceleration
    double speed =  dist / ((double)deceleration * decelerationTweaker);

    //make sure the velocity does not exceed the max
    speed = std::min(speed, mPlayer->maxSpeed());

    //from here proceed just like seek except we don't need to normalize
    //the toTarget vector because we have already gone to the trouble
    //of calculating its length: dist.
    Vector2D desiredVelocity =  toTarget * speed / dist;

    return (desiredVelocity - mPlayer->velocity());
  }

  return Vector2D(0,0);
}


//------------------------------ pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the
//  ball
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::pursuit(const SoccerBall* ball)
{
  Vector2D toBall = ball->pos() - mPlayer->pos();

  //the lookahead time is proportional to the distance between the ball
  //and the pursuer;
  double lookAheadTime = 0.0;

  if (ball->speed() != 0.0)
  {
    lookAheadTime = toBall.length() / ball->speed();
  }

  //calculate where the ball will be at this time in the future
  mTarget = ball->futurePosition(lookAheadTime);

  //now seekBehavior to the predicted future position of the ball
  return arrive(mTarget, fast);
}


//-------------------------- findNeighbours ------------------------------
//
//  tags any vehicles within a predefined radius
//------------------------------------------------------------------------
void SteeringBehaviors::findNeighbours()
{
  std::list<EntityPlayer*>& allPlayers = AutoList<EntityPlayer>::getAllMembers();
  std::list<EntityPlayer*>::iterator curPlyr;
  for (curPlyr = allPlayers.begin(); curPlyr!=allPlayers.end(); ++curPlyr)
  {
    //first clear any current tag
    (*curPlyr)->steering()->unTag();

    //work in distance squared to avoid sqrts
    Vector2D to = (*curPlyr)->pos() - mPlayer->pos();

    if (to.lengthSq() < (mViewDistance * mViewDistance))
    {
      (*curPlyr)->steering()->tag();
    }
  }//next
}


//---------------------------- separation --------------------------------
//
// this calculates a force repelling from the other neighbors
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::separation()
{
   //iterate through all the neighbors and calculate the vector from the
  Vector2D steeringForce;

  std::list<EntityPlayer*>& allPlayers = AutoList<EntityPlayer>::getAllMembers();
  std::list<EntityPlayer*>::iterator curPlyr;
  for (curPlyr = allPlayers.begin(); curPlyr!=allPlayers.end(); ++curPlyr)
  {
    //make sure this agent isn't included in the calculations and that
    //the agent is close enough
    if((*curPlyr != mPlayer) && (*curPlyr)->steering()->tagged())
    {
      Vector2D toAgent = mPlayer->pos() - (*curPlyr)->pos();

      //scale the force inversely proportional to the agents distance
      //from its neighbor.
      steeringForce += vec2DNormalize(toAgent)/toAgent.length();
    }
  }

  return steeringForce;
}


//--------------------------- interpose ----------------------------------
//
//  Given an opponent and an object position this method returns a
//  force that attempts to position the agent between them
//------------------------------------------------------------------------
Vector2D SteeringBehaviors::interpose(const SoccerBall* ball,
                                      Vector2D  target,
                                      double     distFromTarget)
{
  return arrive(target + vec2DNormalize(ball->pos() - target) *
                distFromTarget, normal);
}


//----------------------------- renderAids -------------------------------
//
//------------------------------------------------------------------------
void SteeringBehaviors::renderAids( )
{
  //render the steering force
  gdi->redPen();

  gdi->line(mPlayer->pos(), mPlayer->pos() + mSteeringForce * 20);



}


