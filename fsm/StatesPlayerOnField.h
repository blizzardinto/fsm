#ifndef FIELDPLAYERSTATES_H
#define FIELDPLAYERSTATES_H
#include <string>

#include "State.h"
#include "Telegram.h"
#include "constants.h"


class EntityPlayerOnField;
class SoccerPitch;


//------------------------------------------------------------------------
class GlobalPlayerState : public State<EntityPlayerOnField>
{
private:
  
  GlobalPlayerState(){}

public:

  //this is a singleton
  static GlobalPlayerState* Instance();

  void Enter(EntityPlayerOnField* player){}

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player){}

  bool OnMessage(EntityPlayerOnField*, const Telegram&);
};

//------------------------------------------------------------------------
class ChaseBall : public State<EntityPlayerOnField>
{
private:
  
  ChaseBall(){}

public:

  //this is a singleton
  static ChaseBall* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player);

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Dribble : public State<EntityPlayerOnField>
{
private:
  
  Dribble(){}

public:

  //this is a singleton
  static Dribble* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player){}

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};


//------------------------------------------------------------------------
class ReturnToHomeRegion: public State<EntityPlayerOnField>
{
private:
  
  ReturnToHomeRegion(){}

public:

  //this is a singleton
  static ReturnToHomeRegion* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player);

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Wait: public State<EntityPlayerOnField>
{
private:
  
  Wait(){}

public:

  //this is a singleton
  static Wait* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player);

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class KickBall: public State<EntityPlayerOnField>
{
private:
  
  KickBall(){}

public:

  //this is a singleton
  static KickBall* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player){}

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class ReceiveBall: public State<EntityPlayerOnField>
{
private:
  
  ReceiveBall(){}

public:

  //this is a singleton
  static ReceiveBall* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player);

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};


//------------------------------------------------------------------------
class SupportAttacker: public State<EntityPlayerOnField>
{
private:
  
  SupportAttacker(){}

public:

  //this is a singleton
  static SupportAttacker* Instance();

  void Enter(EntityPlayerOnField* player);

  void Execute(EntityPlayerOnField* player);

  void Exit(EntityPlayerOnField* player);

  bool OnMessage(EntityPlayerOnField*, const Telegram&){return false;}
};




  
#endif