#pragma warning (disable:4786)
#ifndef PLAYERBASE_H
#define PLAYERBASE_H
//------------------------------------------------------------------------
//
//  name: EntityPlayer.h
//
//  Desc: Definition of a soccer player base class. The player inherits
//        from the autolist class so that any player created will be
//        automatically added to a list that is easily accesible by any
//        other game objects. (mainly used by the steering behaviors and
//        player state classes)
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <cassert>
#include "autolist.h"
#include "Vector2D.h"
#include "EntityMovable.h"

class SoccerTeam;
class SoccerPitch;
class SoccerBall;
class SteeringBehaviors;
class Region;



class EntityPlayer : public EntityMovable,
                   public AutoList<EntityPlayer>
{

public:

  enum PlayerRole{goalKeeper, attacker, defender};

protected:

  //this player's role in the team
  PlayerRole             mPlayerRole;

  //a pointer to this player's team
  SoccerTeam*             mTeam;

  //the steering behaviors
  SteeringBehaviors*      mSteering;

  //the region that this player is assigned to.
  int                     mHomeRegion;

  //the region this player moves to before kickoff
  int                     mDefaultRegion;

  //the distance to the ball (in squared-space). This value is queried
  //a lot so it's calculated once each time-step and stored here.
  double                   mDistSqToBall;


  //the vertex buffer
  std::vector<Vector2D>   mPlayerVertices;
  //the buffer for the transformed vertices
  std::vector<Vector2D>   mTransformedPlayerVertices;

public:


  EntityPlayer(SoccerTeam*    homeTeam,
             int            homeRegion,
             Vector2D       heading,
             Vector2D       velocity,
             double          mass,
             double          maxForce,
             double          maxSpeed,
             double          maxTurnRate,
             double          scale,
             PlayerRole    role);

  virtual ~EntityPlayer();


  //returns true if there is an opponent within this player's
  //comfort zone
  bool        isThreatened()const;

  //rotates the player to face the ball or the player's current target
  void        trackBall();
  void        trackTarget();

  //this messages the player that is closest to the supporting spot to
  //change state to support the attacking player
  void        findSupport()const;

  //returns true if the ball can be grabbed by the goalkeeper
  bool        ballWithinKeeperRange()const;

  //returns true if the ball is within kicking range
  bool        ballWithinKickingRange()const;

  //returns true if a ball comes within range of a receiver
  bool        ballWithinReceivingRange()const;

  //returns true if the player is located within the boundaries
  //of his home region
  bool        inHomeRegion()const;

  //returns true if this player is ahead of the attacker
  bool        isAheadOfAttacker()const;

  //returns true if a player is located at the designated support spot
  bool        atSupportSpot()const;

  //returns true if the player is located at his steering target
  bool        atTarget()const;

  //returns true if the player is the closest player in his team to
  //the ball
  bool        isClosestTeamMemberToBall()const;

  //returns true if the point specified by 'position' is located in
  //front of the player
  bool        positionInFrontOfPlayer(Vector2D position)const;

  //returns true if the player is the closest player on the pitch to the ball
  bool        isClosestPlayerOnPitchToBall()const;

  //returns true if this player is the controlling player
  bool        isControllingPlayer()const;

  //returns true if the player is located in the designated 'hot region' --
  //the area close to the opponent's goal
  bool        inHotRegion()const;

  PlayerRole role()const{return mPlayerRole;}

  double       distSqToBall()const{return mDistSqToBall;}
  void        setDistSqToBall(double val){mDistSqToBall = val;}

  //calculate distance to opponent's/home goal. Used frequently by the passing
  //methods
  double       distToOppGoal()const;
  double       distToHomeGoal()const;

  void        setDefaultHomeRegion(){mHomeRegion = mDefaultRegion;}

  SoccerBall* const        ball()const;
  SoccerPitch* const       pitch()const;
  SteeringBehaviors*const  steering()const{return mSteering;}
  const Region* const      homeRegion()const;
  void                     setHomeRegion(int newRegion){mHomeRegion = newRegion;}
  SoccerTeam*const         team()const{return mTeam;}

};





#endif