/*
 * 阅读提示：比赛的顶层协调对象，创建并管理区域、球门、足球和两支球队。
 * 一次更新依次推进足球与球队；球队再推进球员。这是对象分工协作，而不是让窗口管理所有细节。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
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

  // 保存球场边界墙壁的容器。
  std::vector<Wall2D>  mWalls;

  // 表示比赛区域边界的矩形对象。
  Region*              mPlayingArea;

  // 战术区域供球队分配球员站位。
  std::vector<Region*> mRegions;

  // 标记是否有守门员持球。
  bool                 mEntityPlayerGoalKeeperHasBall;

  // 标记比赛是否进行中；准备开球时设为假。
  bool                 mGameOn;

  // 暂停标记为真时停止比赛更新。
  bool                 mPaused;

  // 保存窗口客户区宽高。
  int                  mClientWidth,
                       mClientHeight;

  // 创建供球员站位使用的区域对象。
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