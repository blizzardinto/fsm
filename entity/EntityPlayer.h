/*
 * 阅读提示：球员公共基类，在运动能力之上封装球队关系、角色、距离判断和移动行为。
 * 守门员与场上球员继承这些共同能力，同时分别提供自己的更新和 AI 实现。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#pragma warning (disable:4786)
#ifndef PLAYERBASE_H
#define PLAYERBASE_H
// 球员公共基类：保存球队关系和移动能力，并通过自动列表登记所有球员。原作者：Mat Buckland，2003（fup@ai-junkie.com）。
#include <vector>
#include <string>
#include <cassert>
#include "autolist.h"
#include "Vector2D.h"
#include "EntityMovable.h"

class SoccerTeam;
class SoccerPitch;
class SoccerBall;
class SteeringBehaviors;
class Region;



class EntityPlayer : public EntityMovable,
                   public AutoList<EntityPlayer>
{

public:

  // 枚举给角色编号起名字；角色是数据分类，具体更新行为仍由派生类决定。
  enum PlayerRole{goalKeeper, attacker, defender};

protected:

  // 球员在球队中的角色。
  PlayerRole             mPlayerRole;

  // 借用所属球队的指针；球队的生命周期应覆盖球员的使用期。
  SoccerTeam*             mTeam;

  // 球员拥有的移动行为对象，负责把动作目标转换为力。
  SteeringBehaviors*      mSteering;

  // 当前阵型分配给球员的区域编号。
  int                     mHomeRegion;

  // 开球前应返回的默认区域编号。
  int                     mDefaultRegion;

  // 缓存到足球的距离平方，每轮更新计算一次，供多个判断复用。
  double                   mDistSqToBall;


  // 球员轮廓在自身局部坐标中的顶点。
  std::vector<Vector2D>   mPlayerVertices;
  // 经过旋转、缩放和平移后的绘图顶点。
  std::vector<Vector2D>   mTransformedPlayerVertices;

public:


  EntityPlayer(SoccerTeam*    homeTeam,
             int            homeRegion,
             Vector2D       heading,
             Vector2D       velocity,
             double          mass,
             double          maxForce,
             double          maxSpeed,
             double          maxTurnRate,
             double          scale,
             PlayerRole    role);

  virtual ~EntityPlayer();


  // 判断前方是否存在进入舒适距离的对手。
  bool        isThreatened()const;

  // 转向足球或当前移动目标。
  void        trackBall();
  void        trackTarget();

  // 寻找支援者并通过消息让他进入支援状态。
  void        findSupport()const;

  // 判断足球是否在守门员可接住的范围内。
  bool        ballWithinKeeperRange()const;

  // 判断足球是否在球员可踢到的范围内。
  bool        ballWithinKickingRange()const;

  // 判断足球是否接近接球队员。
  bool        ballWithinReceivingRange()const;

  // 判断球员是否位于自己的站位区域内。
  bool        inHomeRegion()const;

  // 判断自己是否比控球队员更靠近对方球门。
  bool        isAheadOfAttacker()const;

  // 判断球员是否已经到达支援位置。
  bool        atSupportSpot()const;

  // 判断球员是否已经到达移动目标。
  bool        atTarget()const;

  // 判断自己是否是本队距离足球最近的球员。
  bool        isClosestTeamMemberToBall()const;

  // 判断指定位置是否在球员前方。
  bool        positionInFrontOfPlayer(Vector2D position)const;

  // 判断自己是否是双方球员中距离足球最近的人。
  bool        isClosestPlayerOnPitchToBall()const;

  // 判断球队登记的控球队员是否就是自己。
  bool        isControllingPlayer()const;

  // 判断自己是否处于靠近对方球门的进攻区域。
  bool        inHotRegion()const;

  PlayerRole role()const{return mPlayerRole;}

  double       distSqToBall()const{return mDistSqToBall;}
  void        setDistSqToBall(double val){mDistSqToBall = val;}

  // 计算到本方或对方球门的距离，供传球决策使用。
  double       distToOppGoal()const;
  double       distToHomeGoal()const;

  void        setDefaultHomeRegion(){mHomeRegion = mDefaultRegion;}

  SoccerBall* const        ball()const;
  SoccerPitch* const       pitch()const;
  SteeringBehaviors*const  steering()const{return mSteering;}
  const Region* const      homeRegion()const;
  void                     setHomeRegion(int newRegion){mHomeRegion = newRegion;}
  SoccerTeam*const         team()const{return mTeam;}

};





#endif