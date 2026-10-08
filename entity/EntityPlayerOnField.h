#pragma warning (disable:4786)
#ifndef FIELDPLAYER_H
#define FIELDPLAYER_H
//------------------------------------------------------------------------
//
//  Name:   EntityPlayerOnField.h
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
  StateMachine<EntityPlayerOnField>*  m_pStateMachine;
  
  //limits the number of kicks a player may take per second
  Regulator*                  m_pKickLimiter;

  
public:

  EntityPlayerOnField(SoccerTeam*    home_team,
             int        home_region,
             State<EntityPlayerOnField>* start_state,
             Vector2D  heading,
             Vector2D      velocity,
             double         mass,
             double         max_force,
             double         max_speed,
             double         max_turn_rate,
             double         scale,
             player_role    role);   
  
  ~EntityPlayerOnField();

  //call this to update the player's position and orientation
  void        Update();   

  void        Render();

  bool        HandleMessage(const Telegram& msg);

  StateMachine<EntityPlayerOnField>* GetFSM()const{return m_pStateMachine;}

  bool        isReadyForNextKick()const{return m_pKickLimiter->isReady();}

         
};




#endif