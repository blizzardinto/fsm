/*
 * 阅读提示：支援位置计算组件，生成候选位置，并根据安全传球、射门和距离进行评分。
 * 球队把这项计算委托给独立对象，减少球队类自身承担的工作。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "SupportSpotCalculator.h"
#include "EntityPlayer.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "constants.h"
#include "regulator.h"
#include "SoccerTeam.h"
#include "ParamLoader.h"
#include "SoccerPitch.h"

#include "DebugConsole.h"

// 析构时释放支援位置更新频率调节器。
SupportSpotCalculator::~SupportSpotCalculator()
{
  delete mRegulator;
}


// 构造时生成候选支援点，并建立更新调节器。
SupportSpotCalculator::SupportSpotCalculator(int           numX,
                                             int           numY,
                                             SoccerTeam*   team):mBestSupportingSpot(NULL),
                                                                  mTeam(team)
{
  const Region* playingField = team->pitch()->playingArea();

  // 在进攻侧生成候选点并保存到容器。
  double heightOfSupportSpotRegion = playingField->height() * 0.8;
  double widthOfSupportSpotRegion  = playingField->width() * 0.9;
  double sliceX = widthOfSupportSpotRegion / numX ;
  double sliceY = heightOfSupportSpotRegion / numY;

  double left  = playingField->left() + (playingField->width()-widthOfSupportSpotRegion)/2.0 + sliceX/2.0;
  double right = playingField->right() - (playingField->width()-widthOfSupportSpotRegion)/2.0 - sliceX/2.0;
  double top   = playingField->top() + (playingField->height()-heightOfSupportSpotRegion)/2.0 + sliceY/2.0;

  for (int x=0; x<(numX/2)-1; ++x)
  {
    for (int y=0; y<numY; ++y)
    {
      if (mTeam->color() == SoccerTeam::blue)
      {
        mSpots.push_back(SupportSpot(Vector2D(left+x*sliceX, top+y*sliceY), 0.0));
      }

      else
      {
        mSpots.push_back(SupportSpot(Vector2D(right-x*sliceX, top+y*sliceY), 0.0));
      }
    }
  }

  // 创建频率调节器，避免每一轮都重新评分。
  mRegulator = new Regulator(prm.supportSpotUpdateFreq);
}


// 为候选支援点评分并返回最高分位置。
Vector2D SupportSpotCalculator::determineBestSupportingPosition()
{
  // 尚未到更新时间时复用已有的最佳点。
  if (!mRegulator->isReady() && mBestSupportingSpot)
  {
    return mBestSupportingSpot->mPos;
  }

  // 开始新一轮计算前清空最佳点记录。
  mBestSupportingSpot = NULL;

  double bestScoreSoFar = 0.0;

  std::vector<SupportSpot>::iterator curSpot;

  for (curSpot = mSpots.begin(); curSpot != mSpots.end(); ++curSpot)
  {
    // 每个点的初始分数设为一，便于调试绘图显示全部候选点。
    curSpot->mScore = 1.0;

    // 第一项评分：控球队员能否安全地把球传到这里。
    if(mTeam->isPassSafeFromAllOpponents(mTeam->controllingPlayer()->pos(),
                                           curSpot->mPos,
                                           NULL,
                                           prm.maxPassingForce))
    {
      curSpot->mScore += prm.spotPassSafeScore;
    }


    // 第二项评分：从这里能否安全射门。
    if( mTeam->canShoot(curSpot->mPos,
                          prm.maxShootingForce))
    {
      curSpot->mScore += prm.spotCanScoreFromPositionScore;
    }


    // 第三项评分：与控球队员的距离是否接近理想支援距离。
    if (mTeam->supportingPlayer())
    {
      const double optimalDistance = 200.0;

      double dist = vec2DDistance(mTeam->controllingPlayer()->pos(),
                                 curSpot->mPos);

      double temp = fabs(optimalDistance - dist);

      if (temp < optimalDistance)
      {

        // 把距离接近理想值的程度换算成分数。
        curSpot->mScore += prm.spotDistFromControllingPlayerScore *
                             (optimalDistance-temp)/optimalDistance;
      }
    }

    // 分数超过已有最佳点时更新记录。
    if (curSpot->mScore > bestScoreSoFar)
    {
      bestScoreSoFar = curSpot->mScore;

      mBestSupportingSpot = &(*curSpot);
    }

  }

  return mBestSupportingSpot->mPos;
}





// 获取最佳支援点，尚未计算时先执行评分。
Vector2D SupportSpotCalculator::getBestSupportingSpot()
{
  if (mBestSupportingSpot)
  {
    return mBestSupportingSpot->mPos;
  }

  else
  {
    return determineBestSupportingPosition();
  }
}

// 绘制候选点及最佳支援点。
void SupportSpotCalculator::render()const
{
    gdi->hollowBrush();
    gdi->greyPen();

    for (unsigned int spt=0; spt<mSpots.size(); ++spt)
    {
      gdi->circle(mSpots[spt].mPos, mSpots[spt].mScore);
    }

    if (mBestSupportingSpot)
    {
      gdi->greenPen();
      gdi->circle(mBestSupportingSpot->mPos, mBestSupportingSpot->mScore);
    }
}