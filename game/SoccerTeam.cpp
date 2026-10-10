#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "Goal.h"
#include "EntityPlayer.h"
#include "EntityPlayerGoalKeeper.h"
#include "EntityPlayerOnField.h"
#include "utils.h"
#include "SteeringBehaviors.h"
#include "StatesPlayerGoalKeeper.h"
#include "StatesPlayerOnField.h"
#include "ParamLoader.h"
#include "geometry.h"
#include "EntityManager.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"
#include "StatesTeam.h"
#include "DebugConsole.h"
#include <windows.h>

using std::vector;


//----------------------------- ctor -------------------------------------
//
//------------------------------------------------------------------------
SoccerTeam::SoccerTeam(Goal*        homeGoal,
                       Goal*        opponentsGoal,
                       SoccerPitch* pitch,
                       TeamColor   color):mOpponentsGoal(opponentsGoal),
                                           mHomeGoal(homeGoal),
                                           mOpponents(NULL),
                                           mPitch(pitch),
                                           mColor(color),
                                           mDistSqToBallOfClosestPlayer(0.0),
                                           mSupportingPlayer(NULL),
                                           mReceivingPlayer(NULL),
                                           mControllingPlayer(NULL),
                                           mPlayerClosestToBall(NULL)
{
  //setup the state machine
  mStateMachine = new StateMachine<SoccerTeam>(this);

  mStateMachine->setCurrentState(Defending::instance());
  mStateMachine->setPreviousState(Defending::instance());
  mStateMachine->setGlobalState(NULL);

  //create the players and goalkeeper
  createPlayers();

  //set default steering behaviors
  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    (*it)->steering()->separationOn();
  }

  //create the sweet spot calculator
  mSupportSpotCalc = new SupportSpotCalculator(prm.numSupportSpotsX,
                                                 prm.numSupportSpotsY,
                                                 this);
}

//----------------------- dtor -------------------------------------------
//
//------------------------------------------------------------------------
SoccerTeam::~SoccerTeam()
{
  delete mStateMachine;

  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();
  for (it; it != mPlayers.end(); ++it)
  {
    delete *it;
  }

  delete mSupportSpotCalc;
}

//-------------------------- update --------------------------------------
//
//  iterates through each player's update function and calculates
//  frequently accessed info
//------------------------------------------------------------------------
void SoccerTeam::update()
{
  //this information is used frequently so it's more efficient to
  //calculate it just once each frame
  calculateClosestPlayerToBall();

  //the team state machine switches between attack/defense behavior. It
  //also handles the 'kick off' state where a team must return to their
  //kick off positions before the whistle is blown
  mStateMachine->update();

  //now update each player
  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    (*it)->update();
  }

}


//------------------------ calculateClosestPlayerToBall ------------------
//
//  sets m_iClosestPlayerToBall to the player closest to the ball
//------------------------------------------------------------------------
void SoccerTeam::calculateClosestPlayerToBall()
{
  double closestSoFar = maxFloat;

  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    //calculate the dist. Use the squared value to avoid sqrt
    double dist = vec2DDistanceSq((*it)->pos(), pitch()->ball()->pos());

    //keep a record of this value for each player
    (*it)->setDistSqToBall(dist);

    if (dist < closestSoFar)
    {
      closestSoFar = dist;

      mPlayerClosestToBall = *it;
    }
  }

  mDistSqToBallOfClosestPlayer = closestSoFar;
}


//------------- determineBestSupportingAttacker ------------------------
//
// calculate the closest player to the SupportSpot
//------------------------------------------------------------------------
EntityPlayer* SoccerTeam::determineBestSupportingAttacker()
{
  double closestSoFar = maxFloat;

  EntityPlayer* bestPlayer = NULL;

  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    //only attackers utilize the BestSupportingSpot
    if ( ((*it)->role() == EntityPlayer::attacker) && ((*it) != mControllingPlayer) )
    {
      //calculate the dist. Use the squared value to avoid sqrt
      double dist = vec2DDistanceSq((*it)->pos(), mSupportSpotCalc->getBestSupportingSpot());

      //if the distance is the closest so far and the player is not a
      //goalkeeper and the player is not the one currently controlling
      //the ball, keep a record of this player
      if ((dist < closestSoFar) )
      {
        closestSoFar = dist;

        bestPlayer = (*it);
      }
    }
  }

  return bestPlayer;
}

//-------------------------- findPass ------------------------------
//
//  The best pass is considered to be the pass that cannot be intercepted
//  by an opponent and that is as far forward of the receiver as possible
//------------------------------------------------------------------------
bool SoccerTeam::findPass(const EntityPlayer*const passer,
                         EntityPlayer*&           receiver,
                         Vector2D&              passTarget,
                         double                  power,
                         double                  minPassingDistance)const
{

  std::vector<EntityPlayer*>::const_iterator curPlyr = members().begin();

  double    closestToGoalSoFar = maxFloat;
  Vector2D target;

  //iterate through all this player's team members and calculate which
  //one is in a position to be passed the ball
  for (curPlyr; curPlyr != members().end(); ++curPlyr)
  {
    //make sure the potential receiver being examined is not this player
    //and that it is further away than the minimum pass distance
    if ( (*curPlyr != passer) &&
        (vec2DDistanceSq(passer->pos(), (*curPlyr)->pos()) >
         minPassingDistance*minPassingDistance))
    {
      if (getBestPassToReceiver(passer, *curPlyr, target, power))
      {
        //if the pass target is the closest to the opponent's goal line found
        // so far, keep a record of it
        double dist2Goal = fabs(target.x - opponentsGoal()->center().x);

        if (dist2Goal < closestToGoalSoFar)
        {
          closestToGoalSoFar = dist2Goal;

          //keep a record of this player
          receiver = *curPlyr;

          //and the target
          passTarget = target;
        }
      }
    }
  }//next team member

  if (receiver) return true;

  else return false;
}


//---------------------- getBestPassToReceiver ---------------------------
//
//  Three potential passes are calculated. One directly toward the receiver's
//  current position and two that are the tangents from the ball position
//  to the circle of radius 'range' from the receiver.
//  These passes are then tested to see if they can be intercepted by an
//  opponent and to make sure they terminate within the playing area. If
//  all the passes are invalidated the function returns false. Otherwise
//  the function returns the pass that takes the ball closest to the
//  opponent's goal area.
//------------------------------------------------------------------------
bool SoccerTeam::getBestPassToReceiver(const EntityPlayer* const passer,
                                       const EntityPlayer* const receiver,
                                       Vector2D&               passTarget,
                                       double                   power)const
{
  //first, calculate how much time it will take for the ball to reach
  //this receiver, if the receiver was to remain motionless
  double time = pitch()->ball()->timeToCoverDistance(pitch()->ball()->pos(),
                                                    receiver->pos(),
                                                    power);

  //return false if ball cannot reach the receiver after having been
  //kicked with the given power
  if (time < 0) return false;

  //the maximum distance the receiver can cover in this time
  double interceptRange = time * receiver->maxSpeed();

  //scale the intercept range
  const double scalingFactor = 0.3;
  interceptRange *= scalingFactor;

  //now calculate the pass targets which are positioned at the intercepts
  //of the tangents from the ball to the receiver's range circle.
  Vector2D ip1, ip2;

  getTangentPoints(receiver->pos(),
                   interceptRange,
                   pitch()->ball()->pos(),
                   ip1,
                   ip2);

  const int numPassesToTry = 3;
  Vector2D passes[numPassesToTry] = {ip1, receiver->pos(), ip2};


  // this pass is the best found so far if it is:
  //
  //  1. Further upfield than the closest valid pass for this receiver
  //     found so far
  //  2. Within the playing area
  //  3. Cannot be intercepted by any opponents

  double closestSoFar = maxFloat;
  bool  bResult      = false;

  for (int pass=0; pass<numPassesToTry; ++pass)
  {
    double dist = fabs(passes[pass].x - opponentsGoal()->center().x);

    if (( dist < closestSoFar) &&
        pitch()->playingArea()->inside(passes[pass]) &&
        isPassSafeFromAllOpponents(pitch()->ball()->pos(),
                                   passes[pass],
                                   receiver,
                                   power))

    {
      closestSoFar = dist;
      passTarget   = passes[pass];
      bResult      = true;
    }
  }

  return bResult;
}

//----------------------- isPassSafeFromOpponent -------------------------
//
//  test if a pass from 'from' to 'to' can be intercepted by an opposing
//  player
//------------------------------------------------------------------------
bool SoccerTeam::isPassSafeFromOpponent(Vector2D    from,
                                        Vector2D    target,
                                        const EntityPlayer* const receiver,
                                        const EntityPlayer* const opp,
                                        double       passingForce)const
{
  //move the opponent into local space.
  Vector2D toTarget = target - from;
  Vector2D toTargetNormalized = vec2DNormalize(toTarget);

  Vector2D localPosOpp = pointToLocalSpace(opp->pos(),
                                         toTargetNormalized,
                                         toTargetNormalized.perp(),
                                         from);

  //if opponent is behind the kicker then pass is considered okay(this is
  //based on the assumption that the ball is going to be kicked with a
  //velocity greater than the opponent's max velocity)
  if ( localPosOpp.x < 0 )
  {
    return true;
  }

  //if the opponent is further away than the target we need to consider if
  //the opponent can reach the position before the receiver.
  if (vec2DDistanceSq(from, target) < vec2DDistanceSq(opp->pos(), from))
  {
    if (receiver)
    {
      if ( vec2DDistanceSq(target, opp->pos())  >
           vec2DDistanceSq(target, receiver->pos()) )
      {
        return true;
      }

      else
      {
        return false;
      }

    }

    else
    {
      return true;
    }
  }

  //calculate how long it takes the ball to cover the distance to the
  //position orthogonal to the opponents position
  double timeForBall =
  pitch()->ball()->timeToCoverDistance(Vector2D(0,0),
                                       Vector2D(localPosOpp.x, 0),
                                       passingForce);

  //now calculate how far the opponent can run in this time
  double reach = opp->maxSpeed() * timeForBall +
                pitch()->ball()->boundingRadius()+
                opp->boundingRadius();

  //if the distance to the opponent's y position is less than his running
  //range plus the radius of the ball and the opponents radius then the
  //ball can be intercepted
  if ( fabs(localPosOpp.y) < reach )
  {
    return false;
  }

  return true;
}

//---------------------- isPassSafeFromAllOpponents ----------------------
//
//  tests a pass from position 'from' to position 'target' against each member
//  of the opposing team. Returns true if the pass can be made without
//  getting intercepted
//------------------------------------------------------------------------
bool SoccerTeam::isPassSafeFromAllOpponents(Vector2D                from,
                                            Vector2D                target,
                                            const EntityPlayer* const receiver,
                                            double     passingForce)const
{
  std::vector<EntityPlayer*>::const_iterator opp = opponents()->members().begin();

  for (opp; opp != opponents()->members().end(); ++opp)
  {
    if (!isPassSafeFromOpponent(from, target, receiver, *opp, passingForce))
    {
      debugOn

      return false;
    }
  }

  return true;
}

//------------------------ canShoot --------------------------------------
//
//  Given a ball position, a kicking power and a reference to a vector2D
//  this function will sample random positions along the opponent's goal-
//  mouth and check to see if a goal can be scored if the ball was to be
//  kicked in that direction with the given power. If a possible shot is
//  found, the function will immediately return true, with the target
//  position stored in the vector shotTarget.
//------------------------------------------------------------------------
bool SoccerTeam::canShoot(Vector2D  ballPos,
                          double     power,
                          Vector2D  shotTarget)const
{
  //the number of randomly created shot targets this method will test
  int numAttempts = prm.numAttemptsToFindValidStrike;

  while (numAttempts--)
  {
    //choose a random position along the opponent's goal mouth. (making
    //sure the ball's radius is taken into account)
    shotTarget = opponentsGoal()->center();

    //the y value of the shot position should lay somewhere between two
    //goalposts (taking into consideration the ball diameter)
    int minYVal = opponentsGoal()->leftPost().y + pitch()->ball()->boundingRadius();
    int maxYVal = opponentsGoal()->rightPost().y - pitch()->ball()->boundingRadius();

    shotTarget.y = (double)randInt(minYVal, maxYVal);

    //make sure striking the ball with the given power is enough to drive
    //the ball over the goal line.
    double time = pitch()->ball()->timeToCoverDistance(ballPos,
                                                      shotTarget,
                                                      power);

    //if it is, this shot is then tested to see if any of the opponents
    //can intercept it.
    if (time >= 0)
    {
      if (isPassSafeFromAllOpponents(ballPos, shotTarget, NULL, power))
      {
        return true;
      }
    }
  }

  return false;
}


//--------------------- returnAllEntityPlayerOnFieldsToHome ---------------------------
//
//  sends a message to all players to return to their home areas forthwith
//------------------------------------------------------------------------
void SoccerTeam::returnAllEntityPlayerOnFieldsToHome()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ((*it)->role() != EntityPlayer::goalKeeper)
    {
      dispatcher->dispatchMsg(sendMsgImmediately,
                            1,
                            (*it)->id(),
                            msgGoHome,
                            NULL);
    }
  }
}


//--------------------------- render -------------------------------------
//
//  renders the players and any team related info
//------------------------------------------------------------------------
void SoccerTeam::render()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    (*it)->render();
  }

  //show the controlling team and player at the top of the display
  if (prm.bShowControllingTeam)
  {
    gdi->textColor(Cgdi::white);

    if ( (color() == blue) && inControl())
    {
      gdi->textAtPos(20,3,"Blue in Control");
    }
    else if ( (color() == red) && inControl())
    {
      gdi->textAtPos(20,3,"Red in Control");
    }
    if (mControllingPlayer != NULL)
    {
      gdi->textAtPos(pitch()->cxClient()-150, 3, "Controlling Player: " + ttos(mControllingPlayer->id()));
    }
  }

  //render the sweet spots
  if (prm.bSupportSpots && inControl())
  {
    mSupportSpotCalc->render();
  }

//#define SHOW_TEAM_STATE
#ifdef SHOW_TEAM_STATE
  if (color() == red)
  {
    gdi->textColor(Cgdi::white);

    if (currentState() == Attacking::instance())
    {
      gdi->textAtPos(160, 20, "Attacking");
    }
    if (currentState() == Defending::instance())
    {
      gdi->textAtPos(160, 20, "Defending");
    }
    if (currentState() == PrepareForKickOff::instance())
    {
      gdi->textAtPos(160, 20, "Kickoff");
    }
  }
  else
  {
    if (currentState() == Attacking::instance())
    {
      gdi->textAtPos(160, pitch()->cyClient()-40, "Attacking");
    }
    if (currentState() == Defending::instance())
    {
      gdi->textAtPos(160, pitch()->cyClient()-40, "Defending");
    }
    if (currentState() == PrepareForKickOff::instance())
    {
      gdi->textAtPos(160, pitch()->cyClient()-40, "Kickoff");
    }
  }
#endif

//#define SHOW_SUPPORTING_PLAYERS_TARGET
#ifdef SHOW_SUPPORTING_PLAYERS_TARGET
  if (mSupportingPlayer)
  {
    gdi->blueBrush();
    gdi->redPen();
    gdi->circle(mSupportingPlayer->steering()->target(), 4);

  }
#endif

}

//------------------------- createPlayers --------------------------------
//
//  creates the players
//------------------------------------------------------------------------
void SoccerTeam::createPlayers()
{
  if (color() == blue)
  {
    //goalkeeper
    mPlayers.push_back(new EntityPlayerGoalKeeper(this,
                               1,
                               TendGoal::instance(),
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale));

    //create the players
    mPlayers.push_back(new EntityPlayerOnField(this,
                               6,
                               Wait::instance(),
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));



        mPlayers.push_back(new EntityPlayerOnField(this,
                               8,
                               Wait::instance(),
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));





        mPlayers.push_back(new EntityPlayerOnField(this,
                               3,
                               Wait::instance(),
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::defender));


        mPlayers.push_back(new EntityPlayerOnField(this,
                               5,
                               Wait::instance(),
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                              EntityPlayer::defender));

  }

  else
  {

     //goalkeeper
    mPlayers.push_back(new EntityPlayerGoalKeeper(this,
                               16,
                               TendGoal::instance(),
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale));


    //create the players
    mPlayers.push_back(new EntityPlayerOnField(this,
                               9,
                               Wait::instance(),
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));

    mPlayers.push_back(new EntityPlayerOnField(this,
                               11,
                               Wait::instance(),
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));



    mPlayers.push_back(new EntityPlayerOnField(this,
                               12,
                               Wait::instance(),
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::defender));


    mPlayers.push_back(new EntityPlayerOnField(this,
                               14,
                               Wait::instance(),
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::defender));

  }

  //register the players with the entity manager
  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    entityMgr->registerEntity(*it);
  }
}


EntityPlayer* SoccerTeam::getPlayerFromId(int id)const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ((*it)->id() == id) return *it;
  }

  return NULL;
}


void SoccerTeam::setPlayerHomeRegion(int plyr, int region)const
{
  assert ( (plyr>=0) && (plyr<mPlayers.size()) );

  mPlayers[plyr]->setHomeRegion(region);
}


//---------------------- updateTargetsOfWaitingPlayers ------------------------
//
//
void SoccerTeam::updateTargetsOfWaitingPlayers()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ( (*it)->role() != EntityPlayer::goalKeeper )
    {
      //cast to a field player
      EntityPlayerOnField* plyr = static_cast<EntityPlayerOnField*>(*it);

      if ( plyr->getFsm()->isInState(*Wait::instance()) ||
           plyr->getFsm()->isInState(*ReturnToHomeRegion::instance()) )
      {
        plyr->steering()->setTarget(plyr->homeRegion()->center());
      }
    }
  }
}


//--------------------------- allPlayersAtHome --------------------------------
//
//  returns false if any of the team are not located within their home region
//-----------------------------------------------------------------------------
bool SoccerTeam::allPlayersAtHome()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ((*it)->inHomeRegion() == false)
    {
      return false;
    }
  }

  return true;
}

//------------------------- requestPass ---------------------------------------
//
//  this tests to see if a pass is possible between the requester and
//  the controlling player. If it is possible a message is sent to the
//  controlling player to pass the ball asap.
//-----------------------------------------------------------------------------
void SoccerTeam::requestPass(EntityPlayerOnField* requester)const
{
  //maybe put a restriction here
  if (randFloat() > 0.1) return;

  if (isPassSafeFromAllOpponents(controllingPlayer()->pos(),
                                 requester->pos(),
                                 requester,
                                 prm.maxPassingForce))
  {

    //tell the player to make the pass
    //let the receiver know a pass is coming
    dispatcher->dispatchMsg(sendMsgImmediately,
                          requester->id(),
                          controllingPlayer()->id(),
                          msgPassToMe,
                          requester);

  }
}


//----------------------------- isOpponentWithinRadius ------------------------
//
//  returns true if an opposing player is within the radius of the position
//  given as a parameter
//-----------------------------------------------------------------------------
bool SoccerTeam::isOpponentWithinRadius(Vector2D pos, double rad)
{
  std::vector<EntityPlayer*>::const_iterator end = opponents()->members().end();
  std::vector<EntityPlayer*>::const_iterator it;

  for (it=opponents()->members().begin(); it !=end; ++it)
  {
    if (vec2DDistanceSq(pos, (*it)->pos()) < rad*rad)
    {
      return true;
    }
  }

  return false;
}
