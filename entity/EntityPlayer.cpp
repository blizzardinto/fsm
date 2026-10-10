#include "EntityPlayer.h"
#include "SteeringBehaviors.h"
#include "Transformations.h"
#include "Geometry.h"
#include "Cgdi.h"
#include "C2DMatrix.h"
#include "Region.h"
#include "ParamLoader.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"
#include "SoccerTeam.h"
#include "ParamLoader.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "SoccerPitch.h"
#include "DebugConsole.h"


using std::vector;


//----------------------------- dtor -------------------------------------
//------------------------------------------------------------------------
EntityPlayer::~EntityPlayer()
{
  delete mSteering;
}

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
EntityPlayer::EntityPlayer(SoccerTeam* homeTeam,
                       int   homeRegion,
                       Vector2D  heading,
                       Vector2D velocity,
                       double    mass,
                       double    maxForce,
                       double    maxSpeed,
                       double    maxTurnRate,
                       double    scale,
                       PlayerRole role):

    EntityMovable(homeTeam->pitch()->getRegionFromIndex(homeRegion)->center(),
                 scale*10.0,
                 velocity,
                 maxSpeed,
                 heading,
                 mass,
                 Vector2D(scale,scale),
                 maxTurnRate,
                 maxForce),
   mTeam(homeTeam),
   mDistSqToBall(maxFloat),
   mHomeRegion(homeRegion),
   mDefaultRegion(homeRegion),
   mPlayerRole(role)
{

  //setup the vertex buffers and calculate the bounding radius
  const int numPlayerVerts = 4;
  const Vector2D player[numPlayerVerts] = {Vector2D(-3, 8),
                                            Vector2D(3,10),
                                            Vector2D(3,-10),
                                            Vector2D(-3,-8)};

  for (int vtx=0; vtx<numPlayerVerts; ++vtx)
  {
    mPlayerVertices.push_back(player[vtx]);

    //set the bounding radius to the length of the
    //greatest extent
    if (abs(player[vtx].x) > mBoundingRadius)
    {
      mBoundingRadius = abs(player[vtx].x);
    }

    if (abs(player[vtx].y) > mBoundingRadius)
    {
      mBoundingRadius = abs(player[vtx].y);
    }
  }

  //set up the steering behavior class
  mSteering = new SteeringBehaviors(this,
                                      mTeam->pitch(),
                                      ball());

  //a player's start target is its start position (because it's just waiting)
  mSteering->setTarget(homeTeam->pitch()->getRegionFromIndex(homeRegion)->center());
}




//----------------------------- trackBall --------------------------------
//
//  sets the player's heading to point at the ball
//------------------------------------------------------------------------
void EntityPlayer::trackBall()
{
  rotateHeadingToFacePosition(ball()->pos());
}

//----------------------------- trackTarget --------------------------------
//
//  sets the player's heading to point at the current target
//------------------------------------------------------------------------
void EntityPlayer::trackTarget()
{
  setHeading(vec2DNormalize(steering()->target() - pos()));
}


//------------------------------------------------------------------------
//
//binary predicates for std::sort (see CanPassForward/Backward)
//------------------------------------------------------------------------
bool  sortByDistanceToOpponentsGoal(const EntityPlayer*const p1,
                                    const EntityPlayer*const p2)
{
  return (p1->distToOppGoal() < p2->distToOppGoal());
}

bool  sortByReversedDistanceToOpponentsGoal(const EntityPlayer*const p1,
                                            const EntityPlayer*const p2)
{
  return (p1->distToOppGoal() > p2->distToOppGoal());
}


//------------------------- WithinFieldOfView ---------------------------
//
//  returns true if subject is within field of view of this player
//-----------------------------------------------------------------------
bool EntityPlayer::positionInFrontOfPlayer(Vector2D position)const
{
  Vector2D toSubject = position - pos();

  if (toSubject.dot(heading()) > 0)

    return true;

  else

    return false;
}

//------------------------- IsThreatened ---------------------------------
//
//  returns true if there is an opponent within this player's
//  comfort zone
//------------------------------------------------------------------------
bool EntityPlayer::isThreatened()const
{
  //check against all opponents to make sure non are within this
  //player's comfort zone
  std::vector<EntityPlayer*>::const_iterator curOpp;
  curOpp = team()->opponents()->members().begin();

  for (curOpp; curOpp != team()->opponents()->members().end(); ++curOpp)
  {
    //calculate distance to the player. if dist is less than our
    //comfort zone, and the opponent is infront of the player, return true
    if (positionInFrontOfPlayer((*curOpp)->pos()) &&
       (vec2DDistanceSq(pos(), (*curOpp)->pos()) < prm.playerComfortZoneSq))
    {
      return true;
    }

  }// next opp

  return false;
}

//----------------------------- findSupport -----------------------------------
//
//  determines the player who is closest to the SupportSpot and messages him
//  to tell him to change state to SupportAttacker
//-----------------------------------------------------------------------------
void EntityPlayer::findSupport()const
{
  //if there is no support we need to find a suitable player.
  if (team()->supportingPlayer() == NULL)
  {
    EntityPlayer* bestSupportPly = team()->determineBestSupportingAttacker();

    team()->setSupportingPlayer(bestSupportPly);

    dispatcher->dispatchMsg(sendMsgImmediately,
                            id(),
                            team()->supportingPlayer()->id(),
                            msgSupportAttacker,
                            NULL);
  }

  EntityPlayer* bestSupportPly = team()->determineBestSupportingAttacker();

  //if the best player available to support the attacker changes, update
  //the pointers and send messages to the relevant players to update their
  //states
  if (bestSupportPly && (bestSupportPly != team()->supportingPlayer()))
  {

    if (team()->supportingPlayer())
    {
      dispatcher->dispatchMsg(sendMsgImmediately,
                              id(),
                              team()->supportingPlayer()->id(),
                              msgGoHome,
                              NULL);
    }



    team()->setSupportingPlayer(bestSupportPly);

    dispatcher->dispatchMsg(sendMsgImmediately,
                            id(),
                            team()->supportingPlayer()->id(),
                            msgSupportAttacker,
                            NULL);
  }
}


  //calculate distance to opponent's goal. Used frequently by the passing//methods
double EntityPlayer::distToOppGoal()const
{
  return fabs(pos().x - team()->opponentsGoal()->center().x);
}

double EntityPlayer::distToHomeGoal()const
{
  return fabs(pos().x - team()->homeGoal()->center().x);
}

bool EntityPlayer::isControllingPlayer()const
{return team()->controllingPlayer()==this;}

bool EntityPlayer::ballWithinKeeperRange()const
{
  return (vec2DDistanceSq(pos(), ball()->pos()) < prm.keeperInBallRangeSq);
}

bool EntityPlayer::ballWithinReceivingRange()const
{
  return (vec2DDistanceSq(pos(), ball()->pos()) < prm.ballWithinReceivingRangeSq);
}

bool EntityPlayer::ballWithinKickingRange()const
{
  return (vec2DDistanceSq(ball()->pos(), pos()) < prm.playerKickingDistanceSq);
}


bool EntityPlayer::inHomeRegion()const
{
  if (mPlayerRole == goalKeeper)
  {
    return pitch()->getRegionFromIndex(mHomeRegion)->inside(pos(), Region::normal);
  }
  else
  {
    return pitch()->getRegionFromIndex(mHomeRegion)->inside(pos(), Region::halfsize);
  }
}

bool EntityPlayer::atTarget()const
{
  return (vec2DDistanceSq(pos(), steering()->target()) < prm.playerInTargetRangeSq);
}

bool EntityPlayer::isClosestTeamMemberToBall()const
{
  return team()->playerClosestToBall() == this;
}

bool EntityPlayer::isClosestPlayerOnPitchToBall()const
{
  return isClosestTeamMemberToBall() &&
         (distSqToBall() < team()->opponents()->closestDistToBallSq());
}

bool EntityPlayer::inHotRegion()const
{
  return fabs(pos().y - team()->opponentsGoal()->center().y ) <
         pitch()->playingArea()->length()/3.0;
}

bool EntityPlayer::isAheadOfAttacker()const
{
  return fabs(pos().x - team()->opponentsGoal()->center().x) <
         fabs(team()->controllingPlayer()->pos().x - team()->opponentsGoal()->center().x);
}

SoccerBall* const EntityPlayer::ball()const
{
  return team()->pitch()->ball();
}

SoccerPitch* const EntityPlayer::pitch()const
{
  return team()->pitch();
}

const Region* const EntityPlayer::homeRegion()const
{
  return pitch()->getRegionFromIndex(mHomeRegion);
}


