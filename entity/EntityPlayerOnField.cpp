#include "EntityPlayerOnField.h"
#include "EntityPlayer.h"
#include "StatesPlayerOnField.h"
#include "SteeringBehaviors.h"
#include "Transformations.h"
#include "Geometry.h"
#include "Cgdi.h"
#include "C2DMatrix.h"
#include "Goal.h"
#include "Region.h"
#include "EntityFunctionTemplates.h"
#include "ParamLoader.h"
#include "SoccerTeam.h"
#include "Regulator.h"
#include "DebugConsole.h"


#include <limits>

using std::vector;

//------------------------------- dtor ---------------------------------------
//----------------------------------------------------------------------------
EntityPlayerOnField::~EntityPlayerOnField()
{
  delete mKickLimiter;
  delete mStateMachine;
}

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
EntityPlayerOnField::EntityPlayerOnField(SoccerTeam* homeTeam,
                      int   homeRegion,
                      State<EntityPlayerOnField>* startState,
                      Vector2D  heading,
                      Vector2D velocity,
                      double    mass,
                      double    maxForce,
                      double    maxSpeed,
                      double    maxTurnRate,
                      double    scale,
                      PlayerRole role): EntityPlayer(homeTeam,
                                                    homeRegion,
                                                    heading,
                                                    velocity,
                                                    mass,
                                                    maxForce,
                                                    maxSpeed,
                                                    maxTurnRate,
                                                    scale,
                                                    role)
{
  //set up the state machine
  mStateMachine =  new StateMachine<EntityPlayerOnField>(this);

  if (startState)
  {
    mStateMachine->setCurrentState(startState);
    mStateMachine->setPreviousState(startState);
    mStateMachine->setGlobalState(GlobalPlayerState::instance());

    mStateMachine->currentState()->enter(this);
  }

  mSteering->separationOn();

  //set up the kick regulator
  mKickLimiter = new Regulator(prm.playerKickFrequency);
}

//------------------------------ update ----------------------------------
//
//
//------------------------------------------------------------------------
void EntityPlayerOnField::update()
{
  //run the logic for the current state
  mStateMachine->update();

  //calculate the combined steering force
  mSteering->calculate();

  //if no steering force is produced decelerate the player by applying a
  //braking force
  if (mSteering->force().isZero())
  {
    const double brakingRate = 0.8;

    mVelocity = mVelocity * brakingRate;
  }

  //the steering force's side component is a force that rotates the
  //player about its axis. We must limit the rotation so that a player
  //can only turn by playerMaxTurnRate rads per update.
  double turningForce =   mSteering->sideComponent();

  clamp(turningForce, -prm.playerMaxTurnRate, prm.playerMaxTurnRate);

  //rotate the heading vector
  vec2DRotateAroundOrigin(mHeading, turningForce);

  //make sure the velocity vector points in the same direction as
  //the heading vector
  mVelocity = mHeading * mVelocity.length();

  //and recreate mSide
  mSide = mHeading.perp();


  //now to calculate the acceleration due to the force exerted by
  //the forward component of the steering force in the direction
  //of the player's heading
  Vector2D accel = mHeading * mSteering->forwardComponent() / mMass;

  mVelocity += accel;

  //make sure player does not exceed maximum velocity
  mVelocity.truncate(mMaxSpeed);

  //update the position
  mPosition += mVelocity;


  //enforce a non-penetration constraint if desired
  if(prm.bNonPenetrationConstraint)
  {
    enforceNonPenetrationContraint(this, AutoList<EntityPlayer>::getAllMembers());
  }
}

//-------------------- handleMessage -------------------------------------
//
//  routes any messages appropriately
//------------------------------------------------------------------------
bool EntityPlayerOnField::handleMessage(const Telegram& msg)
{
  return mStateMachine->handleMessage(msg);
}

//--------------------------- render -------------------------------------
//
//------------------------------------------------------------------------
void EntityPlayerOnField::render()
{
  gdi->transparentText();
  gdi->textColor(Cgdi::grey);

  //set appropriate team color
  if (team()->color() == SoccerTeam::blue){gdi->bluePen();}
  else{gdi->redPen();}



  //render the player's body
  mTransformedPlayerVertices = worldTransform(mPlayerVertices,
                                         pos(),
                                         heading(),
                                         side(),
                                         scale());
  gdi->closedShape(mTransformedPlayerVertices);

  //and 'is 'ead
  gdi->brownBrush();
  if (prm.bHighlightIfThreatened && (team()->controllingPlayer() == this) && isThreatened()) gdi->yellowBrush();
  gdi->circle(pos(), 6);


  //render the state
  if (prm.bStates)
  {
    gdi->textColor(0, 170, 0);
    gdi->textAtPos(mPosition.x, mPosition.y -20, std::string(mStateMachine->getNameOfCurrentState()));
  }

  //show IDs
  if (prm.bIds)
  {
    gdi->textColor(0, 170, 0);
    gdi->textAtPos(pos().x-20, pos().y-20, ttos(id()));
  }


  if (prm.bViewTargets)
  {
    gdi->redBrush();
    gdi->circle(steering()->target(), 3);
    gdi->textAtPos(steering()->target(), ttos(id()));
  }
}



