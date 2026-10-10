#include "StatesTeam.h"
#include "SoccerTeam.h"
#include "EntityPlayer.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"
#include "constants.h"
#include "SoccerPitch.h"

//uncomment to send state info to debug window
//#define DEBUG_TEAM_STATES
#include "DebugConsole.h"




void changePlayerHomeRegions(SoccerTeam* team, const int newRegions[teamSize])
{
  for (int plyr=0; plyr<teamSize; ++plyr)
  {
    team->setPlayerHomeRegion(plyr, newRegions[plyr]);
  }
}

//************************************************************************ ATTACKING

Attacking* Attacking::instance()
{
  static Attacking instance;

  return &instance;
}


void Attacking::enter(SoccerTeam* team)
{
#ifdef DEBUG_TEAM_STATES
  debugCon << team->name() << " entering Attacking state" << "";
#endif

  //these define the home regions for this state of each of the players
  const int blueRegions[teamSize] = {1,12,14,6,4};
  const int redRegions[teamSize] = {16,3,5,9,13};

  //set up the player's home regions
  if (team->color() == SoccerTeam::blue)
  {
    changePlayerHomeRegions(team, blueRegions);
  }
  else
  {
    changePlayerHomeRegions(team, redRegions);
  }

  //if a player is in either the Wait or ReturnToHomeRegion states, its
  //steering target must be updated to that of its new home region to enable
  //it to move into the correct position.
  team->updateTargetsOfWaitingPlayers();
}


void Attacking::execute(SoccerTeam* team)
{
  //if this team is no longer in control change states
  if (!team->inControl())
  {
    team->getFsm()->changeState(Defending::instance()); return;
  }

  //calculate the best position for any supporting attacker to move to
  team->determineBestSupportingPosition();
}

void Attacking::exit(SoccerTeam* team)
{
  //there is no supporting player for defense
  team->setSupportingPlayer(NULL);
}



//************************************************************************ DEFENDING

Defending* Defending::instance()
{
  static Defending instance;

  return &instance;
}

void Defending::enter(SoccerTeam* team)
{
#ifdef DEBUG_TEAM_STATES
  debugCon << team->name() << " entering Defending state" << "";
#endif

  //these define the home regions for this state of each of the players
  const int blueRegions[teamSize] = {1,6,8,3,5};
  const int redRegions[teamSize] = {16,9,11,12,14};

  //set up the player's home regions
  if (team->color() == SoccerTeam::blue)
  {
    changePlayerHomeRegions(team, blueRegions);
  }
  else
  {
    changePlayerHomeRegions(team, redRegions);
  }

  //if a player is in either the Wait or ReturnToHomeRegion states, its
  //steering target must be updated to that of its new home region
  team->updateTargetsOfWaitingPlayers();
}

void Defending::execute(SoccerTeam* team)
{
  //if in control change states
  if (team->inControl())
  {
    team->getFsm()->changeState(Attacking::instance()); return;
  }
}


void Defending::exit(SoccerTeam* team){}


//************************************************************************ KICKOFF
PrepareForKickOff* PrepareForKickOff::instance()
{
  static PrepareForKickOff instance;

  return &instance;
}

void PrepareForKickOff::enter(SoccerTeam* team)
{
  //reset key player pointers
  team->setControllingPlayer(NULL);
  team->setSupportingPlayer(NULL);
  team->setReceiver(NULL);
  team->setPlayerClosestToBall(NULL);

  //send msgGoHome to each player.
  team->returnAllEntityPlayerOnFieldsToHome();
}

void PrepareForKickOff::execute(SoccerTeam* team)
{
  //if both teams in position, start the game
  if (team->allPlayersAtHome() && team->opponents()->allPlayersAtHome())
  {
    team->getFsm()->changeState(Defending::instance());
  }
}

void PrepareForKickOff::exit(SoccerTeam* team)
{
  team->pitch()->setGameOn();
}


