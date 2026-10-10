/*
 * 阅读提示：支援位置计算组件，生成候选位置，并根据安全传球、射门和距离进行评分。
 * 球队把这项计算委托给独立对象，减少球队类自身承担的工作。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef SUPPORTSPOTCALCULATOR
#define SUPPORTSPOTCALCULATOR
#pragma warning (disable:4786)
#include <vector>

#include "Region.h"
#include "Vector2D.h"
#include "Cgdi.h"


class EntityPlayer;
class Goal;
class SoccerBall;
class SoccerTeam;
class Regulator;



//------------------------------------------------------------------------

class SupportSpotCalculator
{
private:

  // 小结构体把每个支援点的位置和分数放在一起。
  struct SupportSpot
  {

    Vector2D  mPos;

    double    mScore;

    SupportSpot(Vector2D pos, double value):mPos(pos),
                                            mScore(value)
    {}
  };

private:


  SoccerTeam*               mTeam;

  std::vector<SupportSpot>  mSpots;

  // 借用容器中最高分元素的指针；容器重新分配可能使此指针失效。
  SupportSpot*              mBestSupportingSpot;

  // 调节器限制支援评分的更新频率，具体频率由参数决定。
  Regulator*                mRegulator;

public:

  SupportSpotCalculator(int numX,
                        int numY,
                        SoccerTeam* team);

  ~SupportSpotCalculator();

  // 候选点分数越高圆越大，最佳点用绿色突出显示。
  void       render()const;

  // 遍历所有候选点并计算评分。
  Vector2D  determineBestSupportingPosition();

  // 有缓存就返回最佳点，否则先计算。
  Vector2D  getBestSupportingSpot();
};


#endif