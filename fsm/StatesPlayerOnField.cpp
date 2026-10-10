#include "StatesPlayerOnField.h"
#include "DebugConsole.h"
#include "SoccerPitch.h"
#include "EntityPlayerOnField.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "geometry.h"
#include "SoccerBall.h"
#include "ParamLoader.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"

#include "Regulator.h"


//uncomment below to send state info to the debug window
#define PLAYER_STATE_INFO_ON


//************************************************************************ Global state

GlobalPlayerState* GlobalPlayerState::instance()
{
  static GlobalPlayerState instance;

  return &instance;
}


void GlobalPlayerState::execute(EntityPlayerOnField* player)
{
  //if a player is in possession and close to the ball reduce his max speed
  if((player->ballWithinReceivingRange()) && (player->isControllingPlayer()))
  {
    player->setMaxSpeed(prm.playerMaxSpeedWithBall);
  }

  else
  {
     player->setMaxSpeed(prm.playerMaxSpeedWithoutBall);
  }

}


bool GlobalPlayerState::onMessage(EntityPlayerOnField* player, const Telegram& telegram)
{
  switch(telegram.msg)
  {
  case msgReceiveBall:
    {
      //set the target
      player->steering()->setTarget(*(static_cast<Vector2D*>(telegram.extraInfo)));

      //change state
      player->getFsm()->changeState(ReceiveBall::instance());

      return true;
    }

    break;

  case msgSupportAttacker:
    {
      //if already supporting just return
      if (player->getFsm()->isInState(*SupportAttacker::instance()))
      {
        return true;
      }

      //set the target to be the best supporting position
      player->steering()->setTarget(player->team()->getSupportSpot());

      //change the state
      player->getFsm()->changeState(SupportAttacker::instance());

      return true;
    }

    break;

 case msgWait:
    {
      //change the state
      player->getFsm()->changeState(Wait::instance());

      return true;
    }

    break;

  case msgGoHome:
    {
      player->setDefaultHomeRegion();

      player->getFsm()->changeState(ReturnToHomeRegion::instance());

      return true;
    }

    break;

  case msgPassToMe:
    {

      //get the position of the player requesting the pass
      EntityPlayerOnField* receiver = static_cast<EntityPlayerOnField*>(telegram.extraInfo);

      #ifdef PLAYER_STATE_INFO_ON
      debugCon << "Player " << player->id() << " received request from " <<
                    receiver->id() << " to make pass" << "";
      #endif

      //if the ball is not within kicking range or their is already a
      //receiving player, this player cannot pass the ball to the player
      //making the request.
      if (player->team()->receiver() != NULL ||
         !player->ballWithinKickingRange() )
      {
        #ifdef PLAYER_STATE_INFO_ON
        debugCon << "Player " << player->id() << " cannot make requested pass <cannot kick ball>" << "";
        #endif

        return true;
      }

      //make the pass
      player->ball()->kick(receiver->pos() - player->ball()->pos(),
                           prm.maxPassingForce);


     #ifdef PLAYER_STATE_INFO_ON
     debugCon << "Player " << player->id() << " Passed ball to requesting player" << "";
     #endif

      //let the receiver know a pass is coming
      Vector2D passTarget = receiver->pos();
      dispatcher->dispatchMsg(sendMsgImmediately,
                              player->id(),
                              receiver->id(),
                              msgReceiveBall,
                              &passTarget);



      //change state
      player->getFsm()->changeState(Wait::instance());

      player->findSupport();

      return true;
    }

    break;

  }//end switch

  return false;
}




//***************************************************************************** CHASEBALL

ChaseBall* ChaseBall::instance()
{
  static ChaseBall instance;

  return &instance;
}


void ChaseBall::enter(EntityPlayerOnField* player)
{
  player->steering()->seekOn();

  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters chase state" << "";
  #endif
}

void ChaseBall::execute(EntityPlayerOnField* player)
{
  //if the ball is within kicking range the player changes state to KickBall.
  if (player->ballWithinKickingRange())
  {
    player->getFsm()->changeState(KickBall::instance());

    return;
  }

  //if the player is the closest player to the ball then he should keep
  //chasing it
  if (player->isClosestTeamMemberToBall())
  {
    player->steering()->setTarget(player->ball()->pos());

    return;
  }

  //if the player is not closest to the ball anymore, he should return back
  //to his home region and wait for another opportunity
  player->getFsm()->changeState(ReturnToHomeRegion::instance());
}


void ChaseBall::exit(EntityPlayerOnField* player)
{
  player->steering()->seekOff();
}



//*****************************************************************************SUPPORT ATTACKING PLAYER

SupportAttacker* SupportAttacker::instance()
{
  static SupportAttacker instance;

  return &instance;
}


void SupportAttacker::enter(EntityPlayerOnField* player)
{
  player->steering()->arriveOn();

  player->steering()->setTarget(player->team()->getSupportSpot());

  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters support state" << "";
  #endif
}

void SupportAttacker::execute(EntityPlayerOnField* player)
{
  //if his team loses control go back home
  if (!player->team()->inControl())
  {
    player->getFsm()->changeState(ReturnToHomeRegion::instance()); return;
  }


  //if the best supporting spot changes, change the steering target
  if (player->team()->getSupportSpot() != player->steering()->target())
  {
    player->steering()->setTarget(player->team()->getSupportSpot());

    player->steering()->arriveOn();
  }

  //if this player has a shot at the goal AND the attacker can pass
  //the ball to him the attacker should pass the ball to this player
  if( player->team()->canShoot(player->pos(),
                               prm.maxShootingForce))
  {
    player->team()->requestPass(player);
  }


  //if this player is located at the support spot and his team still have
  //possession, he should remain still and turn to face the ball
  if (player->atTarget())
  {
    player->steering()->arriveOff();

    //the player should keep his eyes on the ball!
    player->trackBall();

    player->setVelocity(Vector2D(0,0));

    //if not threatened by another player request a pass
    if (!player->isThreatened())
    {
      player->team()->requestPass(player);
    }
  }
}


void SupportAttacker::exit(EntityPlayerOnField* player)
{
  //set supporting player to null so that the team knows it has to
  //determine a new one.
  player->team()->setSupportingPlayer(NULL);

  player->steering()->arriveOff();
}




//************************************************************************ RETURN TO HOME REGION

ReturnToHomeRegion* ReturnToHomeRegion::instance()
{
  static ReturnToHomeRegion instance;

  return &instance;
}


void ReturnToHomeRegion::enter(EntityPlayerOnField* player)
{
  player->steering()->arriveOn();

  if (!player->homeRegion()->inside(player->steering()->target(), Region::halfsize))
  {
    player->steering()->setTarget(player->homeRegion()->center());
  }

  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters ReturnToHome state" << "";
  #endif
}

void ReturnToHomeRegion::execute(EntityPlayerOnField* player)
{
  if (player->pitch()->gameOn())
  {
    //if the ball is nearer this player than any other team member  &&
    //there is not an assigned receiver && the goalkeeper does not gave
    //the ball, go chase it
    if ( player->isClosestTeamMemberToBall() &&
         (player->team()->receiver() == NULL) &&
         !player->pitch()->entityPlayerGoalKeeperHasBall())
    {
      player->getFsm()->changeState(ChaseBall::instance());

      return;
    }
  }

  //if game is on and close enough to home, change state to wait and set the
  //player target to his current position.(so that if he gets jostled out of
  //position he can move back to it)
  if (player->pitch()->gameOn() && player->homeRegion()->inside(player->pos(),
                                                             Region::halfsize))
  {
    player->steering()->setTarget(player->pos());
    player->getFsm()->changeState(Wait::instance());
  }
  //if game is not on the player must return much closer to the center of his
  //home region
  else if(!player->pitch()->gameOn() && player->atTarget())
  {
    player->getFsm()->changeState(Wait::instance());
  }
}

void ReturnToHomeRegion::exit(EntityPlayerOnField* player)
{
  player->steering()->arriveOff();
}




//***************************************************************************** WAIT

Wait* Wait::instance()
{
  static Wait instance;

  return &instance;
}


void Wait::enter(EntityPlayerOnField* player)
{
  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters wait state" << "";
  #endif

  //if the game is not on make sure the target is the center of the player's
  //home region. This is ensure all the players are in the correct positions
  //ready for kick off
  if (!player->pitch()->gameOn())
  {
    player->steering()->setTarget(player->homeRegion()->center());
  }
}

void Wait::execute(EntityPlayerOnField* player)
{
  //if the player has been jostled out of position, get back in position
  if (!player->atTarget())
  {
    player->steering()->arriveOn();

    return;
  }

  else
  {
    player->steering()->arriveOff();

    player->setVelocity(Vector2D(0,0));

    //the player should keep his eyes on the ball!
    player->trackBall();
  }

  //if this player's team is controlling AND this player is not the attacker
  //AND is further up the field than the attacker he should request a pass.
  if ( player->team()->inControl()    &&
     (!player->isControllingPlayer()) &&
       player->isAheadOfAttacker() )
  {
    player->team()->requestPass(player);

    return;
  }

  if (player->pitch()->gameOn())
  {
   //if the ball is nearer this player than any other team member  AND
    //there is not an assigned receiver AND neither goalkeeper has
    //the ball, go chase it
   if (player->isClosestTeamMemberToBall() &&
       player->team()->receiver() == NULL  &&
       !player->pitch()->entityPlayerGoalKeeperHasBall())
   {
     player->getFsm()->changeState(ChaseBall::instance());

     return;
   }
  }
}

void Wait::exit(EntityPlayerOnField* player){}




//************************************************************************ KICK BALL

KickBall* KickBall::instance()
{
  static KickBall instance;

  return &instance;
}


void KickBall::enter(EntityPlayerOnField* player)
{
  //let the team know this player is controlling
   player->team()->setControllingPlayer(player);

   //the player can only make so many kick attempts per second.
   if (!player->isReadyForNextKick())
   {
     player->getFsm()->changeState(ChaseBall::instance());
   }


  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters kick state" << "";
  #endif
}

void KickBall::execute(EntityPlayerOnField* player)
{
  //calculate the dot product of the vector pointing to the ball
  //and the player's heading
  Vector2D toBall = player->ball()->pos() - player->pos();
  double   dot    = player->heading().dot(vec2DNormalize(toBall));

  //cannot kick the ball if the goalkeeper is in possession or if it is
  //behind the player or if there is already an assigned receiver. So just
  //continue chasing the ball
  if (player->team()->receiver() != NULL   ||
      player->pitch()->entityPlayerGoalKeeperHasBall() ||
      (dot < 0) )
  {
    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Goaly has ball / ball behind player" << "";
    #endif

    player->getFsm()->changeState(ChaseBall::instance());

    return;
  }

  /* Attempt a shot at the goal */

  //if a shot is possible, this vector will hold the position along the
  //opponent's goal line the player should aim for.
  Vector2D    ballTarget;

  //the dot product is used to adjust the shooting force. The more
  //directly the ball is ahead, the more forceful the kick
  double power = prm.maxShootingForce * dot;

  //if it is determined that the player could score a goal from this position
  //OR if he should just kick the ball anyway, the player will attempt
  //to make the shot
  if (player->team()->canShoot(player->ball()->pos(),
                               power,
                               ballTarget)                   ||
     (randFloat() < prm.chancePlayerAttemptsPotShot))
  {
   #ifdef PLAYER_STATE_INFO_ON
   debugCon << "Player " << player->id() << " attempts a shot at " << ballTarget << "";
   #endif

   //add some noise to the kick. We don't want players who are
   //too accurate! The amount of noise can be adjusted by altering
   //prm.playerKickingAccuracy
   ballTarget = addNoiseToKick(player->ball()->pos(), ballTarget);

   //this is the direction the ball will be kicked in
   Vector2D kickDirection = ballTarget - player->ball()->pos();

   player->ball()->kick(kickDirection, power);

   //change state
   player->getFsm()->changeState(Wait::instance());

   player->findSupport();

   return;
 }


  /* Attempt a pass to a player */

  //if a receiver is found this will point to it
  EntityPlayer* receiver = NULL;

  power = prm.maxPassingForce * dot;

  //test if there are any potential candidates available to receive a pass
  if (player->isThreatened()  &&
      player->team()->findPass(player,
                              receiver,
                              ballTarget,
                              power,
                              prm.minPassDist))
  {
    //add some noise to the kick
    ballTarget = addNoiseToKick(player->ball()->pos(), ballTarget);

    Vector2D kickDirection = ballTarget - player->ball()->pos();

    player->ball()->kick(kickDirection, power);

    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Player " << player->id() << " passes the ball with force " << power << "  to player "
              << receiver->id() << "  Target is " << ballTarget << "";
    #endif


    //let the receiver know a pass is coming
    dispatcher->dispatchMsg(sendMsgImmediately,
                            player->id(),
                            receiver->id(),
                            msgReceiveBall,
                            &ballTarget);


    //the player should wait at his current position unless instruced
    //otherwise
    player->getFsm()->changeState(Wait::instance());

    player->findSupport();

    return;
  }

  //cannot shoot or pass, so dribble the ball upfield
  else
  {
    player->findSupport();

    player->getFsm()->changeState(Dribble::instance());
  }
}


//*************************************************************************** DRIBBLE

Dribble* Dribble::instance()
{
  static Dribble instance;

  return &instance;
}


void Dribble::enter(EntityPlayerOnField* player)
{
  //let the team know this player is controlling
  player->team()->setControllingPlayer(player);

#ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters dribble state" << "";
  #endif
}

void Dribble::execute(EntityPlayerOnField* player)
{
  double dot = player->team()->homeGoal()->facing().dot(player->heading());

  //if the ball is between the player and the home goal, it needs to swivel
  // the ball around by doing multiple small kicks and turns until the player
  //is facing in the correct direction
  if (dot < 0)
  {
    //the player's heading is going to be rotated by a small amount (pi/4)
    //and then the ball will be kicked in that direction
    Vector2D direction = player->heading();

    //calculate the sign (+/-) of the angle between the player heading and the
    //facing direction of the goal so that the player rotates around in the
    //correct direction
    double angle = quarterPi * -1 *
                 player->team()->homeGoal()->facing().sign(player->heading());

    vec2DRotateAroundOrigin(direction, angle);

    //this value works well whjen the player is attempting to control the
    //ball and turn at the same time
    const double kickingForce = 0.8;

    player->ball()->kick(direction, kickingForce);
  }

  //kick the ball down the field
  else
  {
    player->ball()->kick(player->team()->homeGoal()->facing(),
                         prm.maxDribbleForce);
  }

  //the player has kicked the ball so he must now change state to follow it
  player->getFsm()->changeState(ChaseBall::instance());

  return;
}



//************************************************************************     RECEIVEBALL

ReceiveBall* ReceiveBall::instance()
{
  static ReceiveBall instance;

  return &instance;
}


void ReceiveBall::enter(EntityPlayerOnField* player)
{
  //let the team know this player is receiving the ball
  player->team()->setReceiver(player);

  //this player is also now the controlling player
  player->team()->setControllingPlayer(player);

  //there are two types of receive behavior. One uses arriveBehavior to direct
  //the receiver to the position sent by the passer in its telegram. The
  //other uses the pursuitBehavior behavior to pursue the ball.
  //This statement selects between them dependent on the probability
  //chanceOfUsingArriveTypeReceiveBehavior, whether or not an opposing
  //player is close to the receiving player, and whether or not the receiving
  //player is in the opponents 'hot region' (the third of the pitch closest
  //to the opponent's goal
  const double passThreatRadius = 70.0;

  if (( player->inHotRegion() ||
        randFloat() < prm.chanceOfUsingArriveTypeReceiveBehavior) &&
     !player->team()->isOpponentWithinRadius(player->pos(), passThreatRadius))
  {
    player->steering()->arriveOn();

    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Player " << player->id() << " enters receive state (Using Arrive)" << "";
    #endif
  }
  else
  {
    player->steering()->pursuitOn();

    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Player " << player->id() << " enters receive state (Using Pursuit)" << "";
    #endif
  }
}

void ReceiveBall::execute(EntityPlayerOnField* player)
{
  //if the ball comes close enough to the player or if his team lose control
  //he should change state to chase the ball
  if (player->ballWithinReceivingRange() || !player->team()->inControl())
  {
    player->getFsm()->changeState(ChaseBall::instance());

    return;
  }

  if (player->steering()->pursuitIsOn())
  {
    player->steering()->setTarget(player->ball()->pos());
  }

  //if the player has 'arrived' at the steering target he should wait and
  //turn to face the ball
  if (player->atTarget())
  {
    player->steering()->arriveOff();
    player->steering()->pursuitOff();
    player->trackBall();
    player->setVelocity(Vector2D(0,0));
  }
}

void ReceiveBall::exit(EntityPlayerOnField* player)
{
  player->steering()->arriveOff();
  player->steering()->pursuitOff();

  player->team()->setReceiver(NULL);
}








