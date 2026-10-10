#ifndef KEEPERSTATES_H
#define KEEPERSTATES_H
#include <string>
#include "State.h"
#include "Telegram.h"
#include "constants.h"


class EntityPlayerGoalKeeper;
class SoccerPitch;


class GlobalKeeperState: public State<EntityPlayerGoalKeeper>
{
private:

  GlobalKeeperState(){}

public:

  //this is a singleton
  static GlobalKeeperState* instance();

  void enter(EntityPlayerGoalKeeper* keeper){}

  void execute(EntityPlayerGoalKeeper* keeper){}

  void exit(EntityPlayerGoalKeeper* keeper){}

  bool onMessage(EntityPlayerGoalKeeper*, const Telegram&);
};

//-----------------------------------------------------------------------------

class TendGoal: public State<EntityPlayerGoalKeeper>
{
private:

  TendGoal(){}

public:

  //this is a singleton
  static TendGoal* instance();

  void enter(EntityPlayerGoalKeeper* keeper);

  void execute(EntityPlayerGoalKeeper* keeper);

  void exit(EntityPlayerGoalKeeper* keeper);

  bool onMessage(EntityPlayerGoalKeeper*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class InterceptBall: public State<EntityPlayerGoalKeeper>
{
private:

  InterceptBall(){}

public:

  //this is a singleton
  static InterceptBall* instance();

  void enter(EntityPlayerGoalKeeper* keeper);

  void execute(EntityPlayerGoalKeeper* keeper);

  void exit(EntityPlayerGoalKeeper* keeper);

  bool onMessage(EntityPlayerGoalKeeper*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class ReturnHome: public State<EntityPlayerGoalKeeper>
{
private:

  ReturnHome(){}

public:

  //this is a singleton
  static ReturnHome* instance();

  void enter(EntityPlayerGoalKeeper* keeper);

  void execute(EntityPlayerGoalKeeper* keeper);

  void exit(EntityPlayerGoalKeeper* keeper);

  bool onMessage(EntityPlayerGoalKeeper*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class PutBallBackInPlay: public State<EntityPlayerGoalKeeper>
{
private:

  PutBallBackInPlay(){}

public:

  //this is a singleton
  static PutBallBackInPlay* instance();

  void enter(EntityPlayerGoalKeeper* keeper);

  void execute(EntityPlayerGoalKeeper* keeper);

  void exit(EntityPlayerGoalKeeper* keeper){}

  bool onMessage(EntityPlayerGoalKeeper*, const Telegram&){return false;}
};





#endif