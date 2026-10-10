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
  static GlobalPlayerState* instance();

  void enter(EntityPlayerOnField* player){}

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player){}

  bool onMessage(EntityPlayerOnField*, const Telegram&);
};

//------------------------------------------------------------------------
class ChaseBall : public State<EntityPlayerOnField>
{
private:

  ChaseBall(){}

public:

  //this is a singleton
  static ChaseBall* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player);

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Dribble : public State<EntityPlayerOnField>
{
private:

  Dribble(){}

public:

  //this is a singleton
  static Dribble* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player){}

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};


//------------------------------------------------------------------------
class ReturnToHomeRegion: public State<EntityPlayerOnField>
{
private:

  ReturnToHomeRegion(){}

public:

  //this is a singleton
  static ReturnToHomeRegion* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player);

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Wait: public State<EntityPlayerOnField>
{
private:

  Wait(){}

public:

  //this is a singleton
  static Wait* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player);

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class KickBall: public State<EntityPlayerOnField>
{
private:

  KickBall(){}

public:

  //this is a singleton
  static KickBall* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player){}

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class ReceiveBall: public State<EntityPlayerOnField>
{
private:

  ReceiveBall(){}

public:

  //this is a singleton
  static ReceiveBall* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player);

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};


//------------------------------------------------------------------------
class SupportAttacker: public State<EntityPlayerOnField>
{
private:

  SupportAttacker(){}

public:

  //this is a singleton
  static SupportAttacker* instance();

  void enter(EntityPlayerOnField* player);

  void execute(EntityPlayerOnField* player);

  void exit(EntityPlayerOnField* player);

  bool onMessage(EntityPlayerOnField*, const Telegram&){return false;}
};





#endif