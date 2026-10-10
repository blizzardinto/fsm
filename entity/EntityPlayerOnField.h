#pragma warning (disable:4786)
#ifndef FIELDPLAYER_H
#define FIELDPLAYER_H
//------------------------------------------------------------------------
//
//  name:   EntityPlayerOnField.h
//
//  Desc:   Derived from a EntityPlayer, this class encapsulates a player
//          capable of moving around a soccer pitch, kicking, dribbling,
//          shooting etc
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

#include "StatesPlayerGoalKeeper.h"
#include "Vector2D.h"
#include "StateMachine.h"
#include "EntityPlayer.h"
#include "StateMachine.h"
#include "Regulator.h"

class CSteeringBehavior;
class SoccerTeam;
class SoccerPitch;
class Goal;
struct Telegram;


class EntityPlayerOnField : public EntityPlayer
{
private:

   //an instance of the state machine class
  StateMachine<EntityPlayerOnField>*  mStateMachine;

  //limits the number of kicks a player may take per second
  Regulator*                  mKickLimiter;


public:

  EntityPlayerOnField(SoccerTeam*    homeTeam,
             int        homeRegion,
             State<EntityPlayerOnField>* startState,
             Vector2D  heading,
             Vector2D      velocity,
             double         mass,
             double         maxForce,
             double         maxSpeed,
             double         maxTurnRate,
             double         scale,
             PlayerRole    role);

  ~EntityPlayerOnField();

  //call this to update the player's position and orientation
  void        update();

  void        render();

  bool        handleMessage(const Telegram& msg);

  StateMachine<EntityPlayerOnField>* getFsm()const{return mStateMachine;}

  bool        isReadyForNextKick()const{return mKickLimiter->isReady();}


};




#endif