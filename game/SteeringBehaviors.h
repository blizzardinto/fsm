#ifndef SteeringBehaviorsS_H
#define SteeringBehaviorsS_H
#pragma warning (disable:4786)
#include <vector>
#include <windows.h>
#include <string>


#include "Vector2D.h"

class EntityPlayer;
class SoccerPitch;
class SoccerBall;
class CWall;
class CObstacle;


//---------------------------- class details -----------------------------

class SteeringBehaviors
{
private:

  EntityPlayer*   mPlayer;

  SoccerBall*   mBall;

  //the steering force created by the combined effect of all
  //the selected behaviors
  Vector2D     mSteeringForce;

  //the current target (usually the ball or predicted ball position)
  Vector2D     mTarget;

  //the distance the player tries to interposeBehavior from the target
  double        mInterposeDist;

  //multipliers.
  double        mMultSeparation;

  //how far it can 'see'
  double        mViewDistance;


  //binary flags to indicate whether or not a behavior should be active
  int           mFlags;

  enum BehaviorType
  {
    none               = 0x0000,
    seekBehavior               = 0x0001,
    arriveBehavior             = 0x0002,
    separationBehavior         = 0x0004,
    pursuitBehavior            = 0x0008,
    interposeBehavior          = 0x0010
  };

  //used by group behaviors to tag neighbours
  bool         mTagged;

  //arrive makes use of these to determine how quickly a vehicle
  //should decelerate to its target
  enum Deceleration{slow = 3, normal = 2, fast = 1};


  //this behavior moves the agent towards a target position
  Vector2D seek(Vector2D target);

  //this behavior is similar to seekBehavior but it attempts to arriveBehavior
  //at the target with a zero velocity
  Vector2D arrive(Vector2D target, Deceleration decel);

  //This behavior predicts where its prey will be and seeks
  //to that location
  Vector2D pursuit(const SoccerBall* ball);

  Vector2D separation();

  //this attempts to steer the agent to a position between the opponent
  //and the object
  Vector2D interpose(const SoccerBall* ball,
                     Vector2D pos,
                     double    distFromTarget);


  //finds any neighbours within the view radius
  void      findNeighbours();


  //this function tests if a specific bit of mFlags is set
  bool      on(BehaviorType bt){return (mFlags & bt) == bt;}

  bool      accumulateForce(Vector2D &sf, Vector2D forceToAdd);

  Vector2D  sumForces();

  //a vertex buffer to contain the feelers rqd for dribbling
  std::vector<Vector2D> mAntenna;


public:

  SteeringBehaviors(EntityPlayer*       agent,
                    SoccerPitch*  world,
                    SoccerBall*   ball);

  virtual ~SteeringBehaviors(){}


  Vector2D calculate();

  //calculates the component of the steering force that is parallel
  //with the vehicle heading
  double    forwardComponent();

  //calculates the component of the steering force that is perpendicuar
  //with the vehicle heading
  double    sideComponent();

  Vector2D force()const{return mSteeringForce;}

  //renders visual aids and info for seeing how each behavior is
  //calculated
  void      renderInfo();
  void      renderAids();

  Vector2D  target()const{return mTarget;}
  void      setTarget(const Vector2D t){mTarget = t;}

  double     interposeDistance()const{return mInterposeDist;}
  void      setInterposeDistance(double d){mInterposeDist = d;}

  bool      tagged()const{return mTagged;}
  void      tag(){mTagged = true;}
  void      unTag(){mTagged = false;}


  void seekOn(){mFlags |= seekBehavior;}
  void arriveOn(){mFlags |= arriveBehavior;}
  void pursuitOn(){mFlags |= pursuitBehavior;}
  void separationOn(){mFlags |= separationBehavior;}
  void interposeOn(double d){mFlags |= interposeBehavior; mInterposeDist = d;}


  void seekOff()  {if(on(seekBehavior))   mFlags ^=seekBehavior;}
  void arriveOff(){if(on(arriveBehavior)) mFlags ^=arriveBehavior;}
  void pursuitOff(){if(on(pursuitBehavior)) mFlags ^=pursuitBehavior;}
  void separationOff(){if(on(separationBehavior)) mFlags ^=separationBehavior;}
  void interposeOff(){if(on(interposeBehavior)) mFlags ^=interposeBehavior;}


  bool seekIsOn(){return on(seekBehavior);}
  bool arriveIsOn(){return on(arriveBehavior);}
  bool pursuitIsOn(){return on(pursuitBehavior);}
  bool separationIsOn(){return on(separationBehavior);}
  bool interposeIsOn(){return on(interposeBehavior);}

};




#endif