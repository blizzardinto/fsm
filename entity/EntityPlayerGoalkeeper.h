#ifndef GOALY_H
#define GOALY_H
//------------------------------------------------------------------------
//
//  name:   EntityPlayerGoalKeeper.h
//
//  Desc:   class to implement a goalkeeper agent
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "Vector2D.h"
#include "EntityPlayer.h"
#include "StateMachine.h"

class EntityPlayer;


class EntityPlayerGoalKeeper : public EntityPlayer
{
private:

   //an instance of the state machine class
  StateMachine<EntityPlayerGoalKeeper>*  mStateMachine;

  //this vector is updated to point towards the ball and is used when
  //rendering the goalkeeper (instead of the underlaying vehicle's heading)
  //to ensure he always appears to be watching the ball
  Vector2D   mLookAt;

public:

   EntityPlayerGoalKeeper(SoccerTeam*        homeTeam,
              int                homeRegion,
              State<EntityPlayerGoalKeeper>* startState,
              Vector2D           heading,
              Vector2D           velocity,
              double              mass,
              double              maxForce,
              double              maxSpeed,
              double              maxTurnRate,
              double              scale);

   ~EntityPlayerGoalKeeper(){delete mStateMachine;}

   //these must be implemented
   void        update();
   void        render();
   bool        handleMessage(const Telegram& msg);


   //returns true if the ball comes close enough for the keeper to
   //consider intercepting
   bool        ballWithinRangeForIntercept()const;

   //returns true if the keeper has ventured too far away from the goalmouth
   bool        tooFarFromGoalMouth()const;

   //this method is called by the Intercept state to determine the spot
   //along the goalmouth which will act as one of the interposeBehavior targets
   //(the other is the ball).
   //the specific point at the goal line that the keeper is trying to cover
   //is flexible and can move depending on where the ball is on the field.
   //to achieve this we just scale the ball's y value by the ratio of the
   //goal width to playingfield width
   Vector2D    getRearInterposeTarget()const;

   StateMachine<EntityPlayerGoalKeeper>* getFsm()const{return mStateMachine;}


   Vector2D    lookAt()const{return mLookAt;}
   void        setLookAt(Vector2D v){mLookAt=v;}
};



#endif