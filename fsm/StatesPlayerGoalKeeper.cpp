#include "StatesPlayerGoalKeeper.h"
#include "DebugConsole.h"
#include "SoccerPitch.h"
#include "EntityPlayer.h"
#include "EntityPlayerGoalKeeper.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "geometry.h"
#include "EntityPlayerOnField.h"
#include "ParamLoader.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"


//uncomment to send state info to debug window
//#define GOALY_STATE_INFO_ON


//--------------------------- GlobalKeeperState -------------------------------
//-----------------------------------------------------------------------------

GlobalKeeperState* GlobalKeeperState::instance()
{
  static GlobalKeeperState instance;

  return &instance;
}


bool GlobalKeeperState::onMessage(EntityPlayerGoalKeeper* keeper, const Telegram& telegram)
{
  switch(telegram.msg)
  {
    case msgGoHome:
    {
      keeper->setDefaultHomeRegion();

      keeper->getFsm()->changeState(ReturnHome::instance());
    }

    break;

    case msgReceiveBall:
      {
        keeper->getFsm()->changeState(InterceptBall::instance());
      }

      break;

  }//end switch

  return false;
}


//--------------------------- TendGoal -----------------------------------
//
//  This is the main state for the goalkeeper. When in this state he will
//  move left to right across the goalmouth using the 'interposeBehavior' steering
//  behavior to put himself between the ball and the back of the net.
//
//  If the ball comes within the 'goalkeeper range' he moves out of the
//  goalmouth to attempt to intercept it. (see next state)
//------------------------------------------------------------------------

TendGoal* TendGoal::instance()
{
  static TendGoal instance;

  return &instance;
}


void TendGoal::enter(EntityPlayerGoalKeeper* keeper)
{
  //turn interposeBehavior on
  keeper->steering()->interposeOn(prm.entityPlayerGoalKeeperTendingDistance);

  //interposeBehavior will position the agent between the ball position and a target
  //position situated along the goal mouth. This call sets the target
  keeper->steering()->setTarget(keeper->getRearInterposeTarget());
}

void TendGoal::execute(EntityPlayerGoalKeeper* keeper)
{
  //the rear interposeBehavior target will change as the ball's position changes
  //so it must be updated each update-step
  keeper->steering()->setTarget(keeper->getRearInterposeTarget());

  //if the ball comes in range the keeper traps it and then changes state
  //to put the ball back in play
  if (keeper->ballWithinKeeperRange())
  {
    keeper->ball()->trap();

    keeper->pitch()->setEntityPlayerGoalKeeperHasBall(true);

    keeper->getFsm()->changeState(PutBallBackInPlay::instance());

    return;
  }

  //if ball is within a predefined distance, the keeper moves out from
  //position to try and intercept it.
  if (keeper->ballWithinRangeForIntercept() && !keeper->team()->inControl())
  {
    keeper->getFsm()->changeState(InterceptBall::instance());
  }

  //if the keeper has ventured too far away from the goal-line and there
  //is no threat from the opponents he should move back towards it
  if (keeper->tooFarFromGoalMouth() && keeper->team()->inControl())
  {
    keeper->getFsm()->changeState(ReturnHome::instance());

    return;
  }
}


void TendGoal::exit(EntityPlayerGoalKeeper* keeper)
{
  keeper->steering()->interposeOff();
}


//------------------------- ReturnHome: ----------------------------------
//
//  In this state the goalkeeper simply returns back to the center of
//  the goal region before changing state back to TendGoal
//------------------------------------------------------------------------

ReturnHome* ReturnHome::instance()
{
  static ReturnHome instance;

  return &instance;
}


void ReturnHome::enter(EntityPlayerGoalKeeper* keeper)
{
  keeper->steering()->arriveOn();
}

void ReturnHome::execute(EntityPlayerGoalKeeper* keeper)
{
  keeper->steering()->setTarget(keeper->homeRegion()->center());

  //if close enough to home or the opponents get control over the ball,
  //change state to tend goal
  if (keeper->inHomeRegion() || !keeper->team()->inControl())
  {
    keeper->getFsm()->changeState(TendGoal::instance());
  }
}

void ReturnHome::exit(EntityPlayerGoalKeeper* keeper)
{
  keeper->steering()->arriveOff();
}



//----------------- InterceptBall ----------------------------------------
//
//  In this state the GP will attempt to intercept the ball using the
//  pursuitBehavior steering behavior, but he only does so so long as he remains
//  within his home region.
//------------------------------------------------------------------------

InterceptBall* InterceptBall::instance()
{
  static InterceptBall instance;

  return &instance;
}


void InterceptBall::enter(EntityPlayerGoalKeeper* keeper)
{
  keeper->steering()->pursuitOn();

    #ifdef GOALY_STATE_INFO_ON
    debugCon << "Goaly " << keeper->id() << " enters InterceptBall" <<  "";
    #endif
}

void InterceptBall::execute(EntityPlayerGoalKeeper* keeper)
{
  //if the goalkeeper moves to far away from the goal he should return to his
  //home region UNLESS he is the closest player to the ball, in which case,
  //he should keep trying to intercept it.
  if (keeper->tooFarFromGoalMouth() && !keeper->isClosestPlayerOnPitchToBall())
  {
    keeper->getFsm()->changeState(ReturnHome::instance());

    return;
  }

  //if the ball becomes in range of the goalkeeper's hands he traps the
  //ball and puts it back in play
  if (keeper->ballWithinKeeperRange())
  {
    keeper->ball()->trap();

    keeper->pitch()->setEntityPlayerGoalKeeperHasBall(true);

    keeper->getFsm()->changeState(PutBallBackInPlay::instance());

    return;
  }
}

void InterceptBall::exit(EntityPlayerGoalKeeper* keeper)
{
  keeper->steering()->pursuitOff();
}



//--------------------------- PutBallBackInPlay --------------------------
//
//------------------------------------------------------------------------

PutBallBackInPlay* PutBallBackInPlay::instance()
{
  static PutBallBackInPlay instance;

  return &instance;
}

void PutBallBackInPlay::enter(EntityPlayerGoalKeeper* keeper)
{
  //let the team know that the keeper is in control
  keeper->team()->setControllingPlayer(keeper);

  //send all the players home
  keeper->team()->opponents()->returnAllEntityPlayerOnFieldsToHome();
  keeper->team()->returnAllEntityPlayerOnFieldsToHome();
}


void PutBallBackInPlay::execute(EntityPlayerGoalKeeper* keeper)
{
  EntityPlayer*  receiver = NULL;
  Vector2D     ballTarget;

  //test if there are players further forward on the field we might
  //be able to pass to. If so, make a pass.
  if (keeper->team()->findPass(keeper,
                              receiver,
                              ballTarget,
                              prm.maxPassingForce,
                              prm.goalkeeperMinPassDist))
  {
    //make the pass
    keeper->ball()->kick(vec2DNormalize(ballTarget - keeper->ball()->pos()),
                         prm.maxPassingForce);

    //goalkeeper no longer has ball
    keeper->pitch()->setEntityPlayerGoalKeeperHasBall(false);

    //let the receiving player know the ball's comin' at him
    dispatcher->dispatchMsg(sendMsgImmediately,
                          keeper->id(),
                          receiver->id(),
                          msgReceiveBall,
                          &ballTarget);

    //go back to tending the goal
    keeper->getFsm()->changeState(TendGoal::instance());

    return;
  }

  keeper->setVelocity(Vector2D());
}

