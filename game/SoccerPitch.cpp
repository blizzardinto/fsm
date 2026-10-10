/*
 * 阅读提示：比赛的顶层协调对象，创建并管理区域、球门、足球和两支球队。
 * 一次更新依次推进足球与球队；球队再推进球员。这是对象分工协作，而不是让窗口管理所有细节。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "Goal.h"
#include "Region.h"
#include "Transformations.h"
#include "Geometry.h"
#include "SoccerTeam.h"
#include "DebugConsole.h"
#include "EntityManager.h"
#include "ParamLoader.h"
#include "EntityPlayer.h"
#include "FrameCounter.h"

const int numRegionsHorizontal = 6;
const int numRegionsVertical   = 3;

// 球场构造函数创建比赛所需的区域、球门、足球和球队。
SoccerPitch::SoccerPitch(int cx, int cy):mClientWidth(cx),
                                         mClientHeight(cy),
                                         mPaused(false),
                                         mEntityPlayerGoalKeeperHasBall(false),
                                         mRegions(numRegionsHorizontal*numRegionsVertical),
                                         mGameOn(true)
{
  // 定义真正用于比赛的场地区域。
  mPlayingArea = new Region(20, 20, cx-20, cy-20);

  // 将场地划分为战术站位区域。
  createRegions(playingArea()->width() / (double)numRegionsHorizontal,
                playingArea()->height() / (double)numRegionsVertical);

  // 创建两侧球门。
   mRedGoal  = new Goal(Vector2D( mPlayingArea->left(), (cy-prm.goalWidth)/2),
                          Vector2D(mPlayingArea->left(), cy - (cy-prm.goalWidth)/2),
                          Vector2D(1,0));



  mBlueGoal = new Goal( Vector2D( mPlayingArea->right(), (cy-prm.goalWidth)/2),
                          Vector2D(mPlayingArea->right(), cy - (cy-prm.goalWidth)/2),
                          Vector2D(-1,0));


  // 创建足球，并传入边界墙壁容器的引用。
  mBall = new SoccerBall(Vector2D((double)mClientWidth/2.0, (double)mClientHeight/2.0),
                           prm.ballSize,
                           prm.ballMass,
                           mWalls);


  // 创建两支球队。
  mRedTeam  = new SoccerTeam(mRedGoal, mBlueGoal, this, SoccerTeam::red);
  mBlueTeam = new SoccerTeam(mBlueGoal, mRedGoal, this, SoccerTeam::blue);

  // 让双方球队互相保存对手指针；这是协作关系，不表示拥有对手。
  mRedTeam->setOpponents(mBlueTeam);
  mBlueTeam->setOpponents(mRedTeam);

  // 创建场地边界墙壁。
  Vector2D topLeft(mPlayingArea->left(), mPlayingArea->top());
  Vector2D topRight(mPlayingArea->right(), mPlayingArea->top());
  Vector2D bottomRight(mPlayingArea->right(), mPlayingArea->bottom());
  Vector2D bottomLeft(mPlayingArea->left(), mPlayingArea->bottom());

  mWalls.push_back(Wall2D(bottomLeft, mRedGoal->rightPost()));
  mWalls.push_back(Wall2D(mRedGoal->leftPost(), topLeft));
  mWalls.push_back(Wall2D(topLeft, topRight));
  mWalls.push_back(Wall2D(topRight, mBlueGoal->leftPost()));
  mWalls.push_back(Wall2D(mBlueGoal->rightPost(), bottomRight));
  mWalls.push_back(Wall2D(bottomRight, bottomLeft));

  ParamLoader* p = ParamLoader::instance();
}

// 球场析构函数销毁它创建并拥有的比赛对象。
SoccerPitch::~SoccerPitch()
{
  delete mBall;

  delete mRedTeam;
  delete mBlueTeam;

  delete mRedGoal;
  delete mBlueGoal;

  delete mPlayingArea;

  for (unsigned int i=0; i<mRegions.size(); ++i)
  {
    delete mRegions[i];
  }
}

// 使用固定更新频率推进模拟，所以实体更新接口不传时间差；实际频率由参数决定。
// 调用顺序表达分工：球场协调一轮比赛，球队负责队内协作，具体球员负责自己的运动。
void SoccerPitch::update()
{
  if (mPaused) return;

  static int tick = 0;

  // 更新足球的物理运动。
  mBall->update();

  // 更新双方球队，球队再更新自己的球员。
  mRedTeam->update();
  mBlueTeam->update();

  // 进球后重置足球，并让双方进入开球准备状态。
  if (mBlueGoal->scored(mBall) || mRedGoal->scored(mBall))
  {
    mGameOn = false;

    // 将足球放回中心并清除速度。
    mBall->placeAtPosition(Vector2D((double)mClientWidth/2.0, (double)mClientHeight/2.0));

    // 通知双方球队准备重新开球。
    mRedTeam->getAi()->changeState(TeamState::prepareForKickOff);
    mBlueTeam->getAi()->changeState(TeamState::prepareForKickOff);
  }
}

// 创建战术区域，把场地划分为行列网格。
void SoccerPitch::createRegions(double width, double height)
{
  // 区域容器中的写入索引。
  int idx = mRegions.size()-1;

  for (int col=0; col<numRegionsHorizontal; ++col)
  {
    for (int row=0; row<numRegionsVertical; ++row)
    {
      mRegions[idx--] = new Region(playingArea()->left()+col*width,
                                   playingArea()->top()+row*height,
                                   playingArea()->left()+(col+1)*width,
                                   playingArea()->top()+(row+1)*height,
                                   idx);
    }
  }
}


// 绘制整个球场及其拥有的比赛对象。
bool SoccerPitch::render()
{
  // 绘制草地背景。
  gdi->darkGreenPen();
  gdi->darkGreenBrush();
  gdi->rect(0,0,mClientWidth, mClientHeight);

  // 按调试设置绘制区域边界和编号。
  if (prm.bRegions)
  {
    for (unsigned int r=0; r<mRegions.size(); ++r)
    {
      mRegions[r]->render(true);
    }
  }

  // 绘制球门。
  gdi->hollowBrush();
  gdi->redPen();
  gdi->rect(mPlayingArea->left(), (mClientHeight-prm.goalWidth)/2, mPlayingArea->left()+40, mClientHeight - (mClientHeight-prm.goalWidth)/2);

  gdi->bluePen();
  gdi->rect(mPlayingArea->right(), (mClientHeight-prm.goalWidth)/2, mPlayingArea->right()-40, mClientHeight - (mClientHeight-prm.goalWidth)/2);

  // 绘制中线、中圈等场地标记。
  gdi->whitePen();
  gdi->circle(mPlayingArea->center(), mPlayingArea->width() * 0.125);
  gdi->line(mPlayingArea->center().x, mPlayingArea->top(), mPlayingArea->center().x, mPlayingArea->bottom());
  gdi->whiteBrush();
  gdi->circle(mPlayingArea->center(), 2.0);


  // 绘制足球。
  gdi->whitePen();
  gdi->whiteBrush();
  mBall->render();

  // 绘制双方球队。
  mRedTeam->render();
  mBlueTeam->render();

  // 绘制边界墙壁。
  gdi->whitePen();
  for (unsigned int w=0; w<mWalls.size(); ++w)
  {
    mWalls[w].render();
  }

  // 显示双方比分。
  gdi->textColor(Cgdi::red);
  gdi->textAtPos((mClientWidth/2)-50, mClientHeight-18, "Red: " + ttos(mBlueGoal->numGoalsScored()));

  gdi->textColor(Cgdi::blue);
  gdi->textAtPos((mClientWidth/2)+10, mClientHeight-18, "Blue: " + ttos(mRedGoal->numGoalsScored()));

  return true;
}







