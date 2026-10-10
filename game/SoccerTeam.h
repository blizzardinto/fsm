/*
 * 阅读提示：球队聚合对象，拥有球员、球队 AI 和支援位置计算器。
 * 它还借用球场、球门及对手指针；读成员时先区分拥有关系与临时协作关系，才能理解生命周期。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef SOCCERTEAM_H
#define SOCCERTEAM_H
#pragma warning (disable:4786)
#include <vector>

#include "Region.h"
#include "SupportSpotCalculator.h"
#include "TeamAI.h"
#include <memory>

class Goal;
class EntityPlayer;
class EntityPlayerOnField;
class SoccerPitch;
class EntityPlayerGoalKeeper;
class SupportSpotCalculator;





class SoccerTeam
{
public:

  enum TeamColor {blue, red};

private:

   // 球队通过智能指针独占自己的 AI 控制器和状态对象。
  std::unique_ptr<TeamAI> mAi;

  // 球队颜色用于区分阵型和绘图外观。
  TeamColor                mColor;

  // 球队拥有这些球员，析构时逐一销毁；容器保存基类指针以统一访问不同角色。
  std::vector<EntityPlayer*>  mPlayers;

  // 借用所属球场的指针，不负责销毁球场。
  SoccerPitch*              mPitch;

  // 借用本方和对方球门，由球场拥有球门。
  Goal*                     mOpponentsGoal;
  Goal*                     mHomeGoal;

  // 借用对手球队指针，不负责销毁对手。
  SoccerTeam*               mOpponents;

  // 借用控球、支援、接球和最近球员的指针，表示当前协作角色。
  EntityPlayer*               mControllingPlayer;
  EntityPlayer*               mSupportingPlayer;
  EntityPlayer*               mReceivingPlayer;
  EntityPlayer*               mPlayerClosestToBall;

  // 缓存最近球员到足球的距离平方。
  double                     mDistSqToBallOfClosestPlayer;

  // 球队拥有支援位置计算器，用它选择进攻站位。
  SupportSpotCalculator*    mSupportSpotCalc;


  // 创建本队所有球员。
  void createPlayers();

  // 每轮计算距离足球最近的本队球员。
  void calculateClosestPlayerToBall();


public:

  SoccerTeam(Goal*        homeGoal,
             Goal*        opponentsGoal,
             SoccerPitch* pitch,
             TeamColor   color);

  ~SoccerTeam();

  // 球队的构造、析构、更新和绘制接口。
  void        render()const;
  void        update();

  // 通过消息让场上球员返回区域，主要用于守门员持球后的重新组织。
  void        returnAllEntityPlayerOnFieldsToHome()const;

  // 检查是否有安全射门；成功时通过引用返回门线上的目标位置，而非单位方向。
  bool        canShoot(Vector2D  ballPos,
                       double     power,
                       Vector2D  shotTarget = Vector2D())const;

  // 寻找不会被拦截且向前推进较多的传球，通过引用参数返回接球者指针和目标位置。
  bool        findPass(const EntityPlayer*const passer,
                      EntityPlayer*&           receiver,
                      Vector2D&              passTarget,
                      double                  power,
                      double                  minPassingDistance)const;

  // 对接球者当前位置及可达圆的两个切点进行评估，选出场内、安全且更靠近对方门线的目标。
  bool        getBestPassToReceiver(const EntityPlayer* const passer,
                                    const EntityPlayer* const receiver,
                                    Vector2D& passTarget,
                                    const double power)const;

  // 判断一个对手能否拦截给定起点、目标和力量的传球。
  bool        isPassSafeFromOpponent(Vector2D    from,
                                     Vector2D    target,
                                     const EntityPlayer* const receiver,
                                     const EntityPlayer* const opp,
                                     double       passingForce)const;

  // 所有对手都无法拦截时，传球才安全。
  bool        isPassSafeFromAllOpponents(Vector2D from,
                                         Vector2D target,
                                         const EntityPlayer* const receiver,
                                         double     passingForce)const;

  // 判断指定位置的半径范围内是否有对手。
  bool        isOpponentWithinRadius(Vector2D pos, double rad);

  // 传球可行时，向当前控球队员发送请求消息。
  void        requestPass(EntityPlayerOnField* requester)const;

  // 更新最佳支援位置，并选择最合适的支援球员。
  EntityPlayer* determineBestSupportingAttacker();


  const std::vector<EntityPlayer*>& members()const{return mPlayers;}

  TeamAI* getAi()const;

  Goal*const           homeGoal()const{return mHomeGoal;}
  Goal*const           opponentsGoal()const{return mOpponentsGoal;}

  SoccerPitch*const    pitch()const{return mPitch;}

  SoccerTeam*const     opponents()const{return mOpponents;}
  void                 setOpponents(SoccerTeam* opps){mOpponents = opps;}

  TeamColor           color()const{return mColor;}

  void                 setPlayerClosestToBall(EntityPlayer* plyr){mPlayerClosestToBall=plyr;}
  EntityPlayer*          playerClosestToBall()const{return mPlayerClosestToBall;}

  double               closestDistToBallSq()const{return mDistSqToBallOfClosestPlayer;}

  Vector2D             getSupportSpot()const{return mSupportSpotCalc->getBestSupportingSpot();}

  EntityPlayer*          supportingPlayer()const{return mSupportingPlayer;}
  void                 setSupportingPlayer(EntityPlayer* plyr){mSupportingPlayer = plyr;}

  EntityPlayer*          receiver()const{return mReceivingPlayer;}
  void                 setReceiver(EntityPlayer* plyr){mReceivingPlayer = plyr;}

  EntityPlayer*          controllingPlayer()const{return mControllingPlayer;}
  void                 setControllingPlayer(EntityPlayer* plyr)
  {
    mControllingPlayer = plyr;

    // 返回球队名称，供界面和调试输出使用。
    opponents()->lostControl();
  }


  bool  inControl()const{if(mControllingPlayer)return true; else return false;}
  void  lostControl(){mControllingPlayer = NULL;}

  EntityPlayer*  getPlayerFromId(int id)const;


  void setPlayerHomeRegion(int plyr, int region)const;

  void determineBestSupportingPosition()const{mSupportSpotCalc->determineBestSupportingPosition();}

  void updateTargetsOfWaitingPlayers()const;

  // 任何一个球员不在自己的区域内，就返回假。
  bool allPlayersAtHome()const;

  std::string name()const{if (mColor == blue) return "Blue"; return "Red";}

};

#endif
