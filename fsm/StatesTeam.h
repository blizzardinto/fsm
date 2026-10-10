#ifndef TEAMSTATES_H
#define TEAMSTATES_H
#include <string>

#include "State.h"
#include "Telegram.h"


class SoccerTeam;





//------------------------------------------------------------------------
class Attacking : public State<SoccerTeam>
{
private:

  Attacking(){}

public:

  //this is a singleton
  static Attacking* instance();

  void enter(SoccerTeam* team);

  void execute(SoccerTeam* team);

  void exit(SoccerTeam* team);

  bool onMessage(SoccerTeam*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Defending : public State<SoccerTeam>
{
private:

  Defending(){}

public:

    //this is a singleton
  static Defending* instance();

  void enter(SoccerTeam* team);

  void execute(SoccerTeam* team);

  void exit(SoccerTeam* team);

  bool onMessage(SoccerTeam*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class PrepareForKickOff : public State<SoccerTeam>
{
private:

  PrepareForKickOff(){}

public:

    //this is a singleton
  static PrepareForKickOff* instance();

  void enter(SoccerTeam* team);

  void execute(SoccerTeam* team);

  void exit(SoccerTeam* team);

  bool onMessage(SoccerTeam*, const Telegram&){return false;}
};


#endif