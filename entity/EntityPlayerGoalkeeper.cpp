#include "EntityPlayerGoalKeeper.h"
#include "Cgdi.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "transformations.h"
#include "StatesPlayerGoalKeeper.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "EntityFunctionTemplates.h"
#include "ParamLoader.h"



//----------------------------- ctor ------------------------------------
//-----------------------------------------------------------------------
EntityPlayerGoalKeeper::EntityPlayerGoalKeeper(SoccerTeam*        homeTeam,
                       int                homeRegion,
                       State<EntityPlayerGoalKeeper>* startState,
                       Vector2D           heading,
                       Vector2D           velocity,
                       double              mass,
                       double              maxForce,
                       double              maxSpeed,
                       double              maxTurnRate,
                       double              scale): EntityPlayer(homeTeam,
                                                             homeRegion,
                                                             heading,
                                                             velocity,
                                                             mass,
                                                             maxForce,
                                                             maxSpeed,
                                                             maxTurnRate,
                                                             scale,
                                                             EntityPlayer::goalKeeper)


{
   //set up the state machine
  mStateMachine = new StateMachine<EntityPlayerGoalKeeper>(this);

  mStateMachine->setCurrentState(startState);
  mStateMachine->setPreviousState(startState);
  mStateMachine->setGlobalState(GlobalKeeperState::instance());

  mStateMachine->currentState()->enter(this);
}



//-------------------------- update --------------------------------------

void EntityPlayerGoalKeeper::update()
{
  //run the logic for the current state
  mStateMachine->update();

  //calculate the combined force from each steering behavior
  Vector2D steeringForce = mSteering->calculate();



  //acceleration = force/mass
  Vector2D acceleration = steeringForce / mMass;

  //update velocity
  mVelocity += acceleration;

  //make sure player does not exceed maximum velocity
  mVelocity.truncate(mMaxSpeed);

  //update the position
  mPosition += mVelocity;


  //enforce a non-penetration constraint if desired
  if(prm.bNonPenetrationConstraint)
  {
    enforceNonPenetrationContraint(this, AutoList<EntityPlayer>::getAllMembers());
  }

  //update the heading if the player has a non zero velocity
  if ( !mVelocity.isZero())
  {
    mHeading = vec2DNormalize(mVelocity);

    mSide = mHeading.perp();
  }

  //look-at vector always points toward the ball
  if (!pitch()->entityPlayerGoalKeeperHasBall())
  {
   mLookAt = vec2DNormalize(ball()->pos() - pos());
  }
}


bool EntityPlayerGoalKeeper::ballWithinRangeForIntercept()const
{
  return (vec2DDistanceSq(team()->homeGoal()->center(), ball()->pos()) <=
          prm.entityPlayerGoalKeeperInterceptRangeSq);
}

bool EntityPlayerGoalKeeper::tooFarFromGoalMouth()const
{
  return (vec2DDistanceSq(pos(), getRearInterposeTarget()) >
          prm.entityPlayerGoalKeeperInterceptRangeSq);
}

Vector2D EntityPlayerGoalKeeper::getRearInterposeTarget()const
{
  double xPosTarget = team()->homeGoal()->center().x;

  double yPosTarget = pitch()->playingArea()->center().y -
                     prm.goalWidth*0.5 + (ball()->pos().y*prm.goalWidth) /
                     pitch()->playingArea()->height();

  return Vector2D(xPosTarget, yPosTarget);
}

//-------------------- handleMessage -------------------------------------
//
//  routes any messages appropriately
//------------------------------------------------------------------------
bool EntityPlayerGoalKeeper::handleMessage(const Telegram& msg)
{
  return mStateMachine->handleMessage(msg);
}

//--------------------------- render -------------------------------------
//
//------------------------------------------------------------------------
void EntityPlayerGoalKeeper::render()
{
  if (team()->color() == SoccerTeam::blue)
    gdi->bluePen();
  else
    gdi->redPen();

  mTransformedPlayerVertices = worldTransform(mPlayerVertices,
                                       pos(),
                                       mLookAt,
                                       mLookAt.perp(),
                                       scale());

  gdi->closedShape(mTransformedPlayerVertices);

  //draw the head
  gdi->brownBrush();
  gdi->circle(pos(), 6);

  //draw the id
  if (prm.bIds)
  {
    gdi->textColor(0, 170, 0);;
    gdi->textAtPos(pos().x-20, pos().y-20, ttos(id()));
  }

  //draw the state
  if (prm.bStates)
  {
    gdi->textColor(0, 170, 0);
    gdi->transparentText();
    gdi->textAtPos(mPosition.x, mPosition.y -20, std::string(mStateMachine->getNameOfCurrentState()));
  }
}
