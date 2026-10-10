#ifndef SOCCERPITCH_H
#define SOCCERPITCH_H
#pragma warning (disable:4786)
#include <windows.h>
#include <vector>
#include <cassert>

#include "Wall2D.h"
#include "Vector2D.h"
#include "constants.h"

class Region;
class Goal;
class SoccerTeam;
class SoccerBall;
class SoccerTeam;
class EntityPlayer;


class SoccerPitch
{
public:

  SoccerBall*          mBall;

  SoccerTeam*          mRedTeam;
  SoccerTeam*          mBlueTeam;

  Goal*                mRedGoal;
  Goal*                mBlueGoal;

  //container for the boundary walls
  std::vector<Wall2D>  mWalls;

  //defines the dimensions of the playing area
  Region*              mPlayingArea;

  //the playing field is broken up into regions that the team
  //can make use of to implement strategies.
  std::vector<Region*> mRegions;

  //true if a goal keeper has possession
  bool                 mEntityPlayerGoalKeeperHasBall;

  //true if the game is in play. Set to false whenever the players
  //are getting ready for kickoff
  bool                 mGameOn;

  //set true to pause the motion
  bool                 mPaused;

  //local copy of client window dimensions
  int                  mClientWidth,
                       mClientHeight;

  //this instantiates the regions the players utilize to  position
  //themselves
  void createRegions(double width, double height);


public:

  SoccerPitch(int cxClient, int cyClient);

  ~SoccerPitch();

  void  update();

  bool  render();

  void  togglePause(){mPaused = !mPaused;}
  bool  paused()const{return mPaused;}

  int   cxClient()const{return mClientWidth;}
  int   cyClient()const{return mClientHeight;}

  bool  entityPlayerGoalKeeperHasBall()const{return mEntityPlayerGoalKeeperHasBall;}
  void  setEntityPlayerGoalKeeperHasBall(bool b){mEntityPlayerGoalKeeperHasBall = b;}

  const Region*const         playingArea()const{return mPlayingArea;}
  const std::vector<Wall2D>& walls(){return mWalls;}
  SoccerBall*const           ball()const{return mBall;}

  const Region* const getRegionFromIndex(int idx)
  {
    assert ( (idx > 0) && (idx < mRegions.size()) );

    return mRegions[idx];
  }

  bool  gameOn()const{return mGameOn;}
  void  setGameOn(){mGameOn = true;}
  void  setGameOff(){mGameOn = false;}

};

#endif