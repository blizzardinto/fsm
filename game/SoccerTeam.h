#ifndef SOCCERTEAM_H
#define SOCCERTEAM_H
#pragma warning (disable:4786)
#include <vector>

#include "Region.h"
#include "SupportSpotCalculator.h"
#include "StateMachine.h"

class Goal;
class EntityPlayer;
class EntityPlayerOnField;
class SoccerPitch;
class EntityPlayerGoalKeeper;
class SupportSpotCalculator;





class SoccerTeam
{
public:

  enum TeamColor {blue, red};

private:

   //an instance of the state machine class
  StateMachine<SoccerTeam>*  mStateMachine;

  //the team must know its own color!
  TeamColor                mColor;

  //pointers to the team members
  std::vector<EntityPlayer*>  mPlayers;

  //a pointer to the soccer pitch
  SoccerPitch*              mPitch;

  //pointers to the goals
  Goal*                     mOpponentsGoal;
  Goal*                     mHomeGoal;

  //a pointer to the opposing team
  SoccerTeam*               mOpponents;

  //pointers to 'key' players
  EntityPlayer*               mControllingPlayer;
  EntityPlayer*               mSupportingPlayer;
  EntityPlayer*               mReceivingPlayer;
  EntityPlayer*               mPlayerClosestToBall;

  //the squared distance the closest player is from the ball
  double                     mDistSqToBallOfClosestPlayer;

  //players use this to determine strategic positions on the playing field
  SupportSpotCalculator*    mSupportSpotCalc;


  //creates all the players for this team
  void createPlayers();

  //called each frame. Sets m_pClosestPlayerToBall to point to the player
  //closest to the ball.
  void calculateClosestPlayerToBall();


public:

  SoccerTeam(Goal*        homeGoal,
             Goal*        opponentsGoal,
             SoccerPitch* pitch,
             TeamColor   color);

  ~SoccerTeam();

  //the usual suspects
  void        render()const;
  void        update();

  //calling this changes the state of all field players to that of
  //ReturnToHomeRegion. Mainly used when a goal keeper has
  //possession
  void        returnAllEntityPlayerOnFieldsToHome()const;

  //returns true if player has a clean shot at the goal and sets shotTarget
  //to a normalized vector pointing in the direction the shot should be
  //made. Else returns false and sets heading to a zero vector
  bool        canShoot(Vector2D  ballPos,
                       double     power,
                       Vector2D  shotTarget = Vector2D())const;

  //The best pass is considered to be the pass that cannot be intercepted
  //by an opponent and that is as far forward of the receiver as possible
  //If a pass is found, the receiver's address is returned in the
  //reference, 'receiver' and the position the pass will be made to is
  //returned in the  reference 'passTarget'
  bool        findPass(const EntityPlayer*const passer,
                      EntityPlayer*&           receiver,
                      Vector2D&              passTarget,
                      double                  power,
                      double                  minPassingDistance)const;

  //Three potential passes are calculated. One directly toward the receiver's
  //current position and two that are the tangents from the ball position
  //to the circle of radius 'range' from the receiver.
  //These passes are then tested to see if they can be intercepted by an
  //opponent and to make sure they terminate within the playing area. If
  //all the passes are invalidated the function returns false. Otherwise
  //the function returns the pass that takes the ball closest to the
  //opponent's goal area.
  bool        getBestPassToReceiver(const EntityPlayer* const passer,
                                    const EntityPlayer* const receiver,
                                    Vector2D& passTarget,
                                    const double power)const;

  //test if a pass from positions 'from' to 'target' kicked with force
  //'passingForce'can be intercepted by an opposing player
  bool        isPassSafeFromOpponent(Vector2D    from,
                                     Vector2D    target,
                                     const EntityPlayer* const receiver,
                                     const EntityPlayer* const opp,
                                     double       passingForce)const;

  //tests a pass from position 'from' to position 'target' against each member
  //of the opposing team. Returns true if the pass can be made without
  //getting intercepted
  bool        isPassSafeFromAllOpponents(Vector2D from,
                                         Vector2D target,
                                         const EntityPlayer* const receiver,
                                         double     passingForce)const;

  //returns true if there is an opponent within radius of position
  bool        isOpponentWithinRadius(Vector2D pos, double rad);

  //this tests to see if a pass is possible between the requester and
  //the controlling player. If it is possible a message is sent to the
  //controlling player to pass the ball asap.
  void        requestPass(EntityPlayerOnField* requester)const;

  //calculates the best supporting position and finds the most appropriate
  //attacker to travel to the spot
  EntityPlayer* determineBestSupportingAttacker();


  const std::vector<EntityPlayer*>& members()const{return mPlayers;}

  StateMachine<SoccerTeam>* getFsm()const{return mStateMachine;}

  Goal*const           homeGoal()const{return mHomeGoal;}
  Goal*const           opponentsGoal()const{return mOpponentsGoal;}

  SoccerPitch*const    pitch()const{return mPitch;}

  SoccerTeam*const     opponents()const{return mOpponents;}
  void                 setOpponents(SoccerTeam* opps){mOpponents = opps;}

  TeamColor           color()const{return mColor;}

  void                 setPlayerClosestToBall(EntityPlayer* plyr){mPlayerClosestToBall=plyr;}
  EntityPlayer*          playerClosestToBall()const{return mPlayerClosestToBall;}

  double               closestDistToBallSq()const{return mDistSqToBallOfClosestPlayer;}

  Vector2D             getSupportSpot()const{return mSupportSpotCalc->getBestSupportingSpot();}

  EntityPlayer*          supportingPlayer()const{return mSupportingPlayer;}
  void                 setSupportingPlayer(EntityPlayer* plyr){mSupportingPlayer = plyr;}

  EntityPlayer*          receiver()const{return mReceivingPlayer;}
  void                 setReceiver(EntityPlayer* plyr){mReceivingPlayer = plyr;}

  EntityPlayer*          controllingPlayer()const{return mControllingPlayer;}
  void                 setControllingPlayer(EntityPlayer* plyr)
  {
    mControllingPlayer = plyr;

    //rub it in the opponents faces!
    opponents()->lostControl();
  }


  bool  inControl()const{if(mControllingPlayer)return true; else return false;}
  void  lostControl(){mControllingPlayer = NULL;}

  EntityPlayer*  getPlayerFromId(int id)const;


  void setPlayerHomeRegion(int plyr, int region)const;

  void determineBestSupportingPosition()const{mSupportSpotCalc->determineBestSupportingPosition();}

  void updateTargetsOfWaitingPlayers()const;

  //returns false if any of the team are not located within their home region
  bool allPlayersAtHome()const;

  std::string name()const{if (mColor == blue) return "Blue"; return "Red";}

};

#endif