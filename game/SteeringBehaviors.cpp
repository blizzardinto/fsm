/*
 * 阅读提示：移动行为组件，借用一个球员对象，并把目标与行为开关换算成移动力。
 * 组合允许球员复用寻找、到达、追踪、分离和阻挡算法；此组件不负责选择足球战术状态。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "SteeringBehaviors.h"
#include "EntityPlayer.h"
#include "Transformations.h"
#include "utils.h"
#include "SoccerTeam.h"
#include "autolist.h"
#include "ParamLoader.h"
#include "SoccerBall.h"
#include <algorithm>


using std::string;
using std::vector;

// 构造移动行为对象：绑定服务的球员，并初始化各行为权重。
SteeringBehaviors::SteeringBehaviors(EntityPlayer*  agent,
                                     SoccerPitch* world,
                                     SoccerBall*  ball):

             mPlayer(agent),
             mFlags(0),
             mMultSeparation(prm.separationCoefficient),
             mTagged(false),
             mViewDistance(prm.viewDistance),
             mBall(ball),
             mInterposeDist(0.0),
             mAntenna(5,Vector2D())
{
}

// 按剩余驱动力预算累加一个行为的力；预算不足时只加入可用部分。
bool SteeringBehaviors::accumulateForce(Vector2D &sf, Vector2D forceToAdd)
{
  // 计算目前还剩多少可用驱动力。
  double magnitudeSoFar = sf.length();

  double magnitudeRemaining = mPlayer->maxForce() - magnitudeSoFar;

  // 驱动力预算已经用完时返回假。
  if (magnitudeRemaining <= 0.0) return false;

  // 计算准备加入的力的大小。
  double magnitudeToAdd = forceToAdd.length();

  // 只加入预算允许的部分。
  if (magnitudeToAdd > magnitudeRemaining)
  {
    magnitudeToAdd = magnitudeRemaining;
  }

  // 将这一部分力累加到总移动力。
  sf += (vec2DNormalize(forceToAdd) * magnitudeToAdd);

  return true;
}

// 汇总当前已启用的行为，得到球员本轮使用的移动力。
Vector2D SteeringBehaviors::calculate()
{
  // 每轮开始清空上一轮累积的力。
  mSteeringForce.zero();

  // 临时保存单个行为产生的力。
  mSteeringForce = sumForces();

  // 把总力限制在球员最大驱动力范围内。
  mSteeringForce.truncate(mPlayer->maxForce());

  return mSteeringForce;
}

// 按优先顺序累积已启用行为的力，预算用完时停止继续叠加。
Vector2D SteeringBehaviors::sumForces()
{
   Vector2D force;

  // 先标记附近球员，供分离行为使用。
   findNeighbours();

  if (on(separationBehavior))
  {
    force += separation() * mMultSeparation;

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(seekBehavior))
  {
    force += seek(mTarget);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(arriveBehavior))
  {
    force += arrive(mTarget, fast);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(pursuitBehavior))
  {
    force += pursuit(mBall);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  if (on(interposeBehavior))
  {
    force += interpose(mBall, mTarget, mInterposeDist);

    if (!accumulateForce(mSteeringForce, force)) return mSteeringForce;
  }

  return mSteeringForce;
}

// 将移动力投影到朝向，得到前向分量。
double SteeringBehaviors::forwardComponent()
{
  return mPlayer->heading().dot(mSteeringForce);
}

// 将移动力投影到侧向量，得到侧向分量。
double SteeringBehaviors::sideComponent()
{
  return mPlayer->side().dot(mSteeringForce) * mPlayer->maxTurnRate();
}


// 寻找行为：朝目标产生期望速度，再减去当前速度得到调整力。
Vector2D SteeringBehaviors::seek(Vector2D target)
{

  Vector2D desiredVelocity = vec2DNormalize(target - mPlayer->pos())
                            * mPlayer->maxSpeed();

  return (desiredVelocity - mPlayer->velocity());
}


// 到达行为：目标越近速度越低，争取停在目标附近。
Vector2D SteeringBehaviors::arrive(Vector2D    target,
                                   Deceleration deceleration)
{
  Vector2D toTarget = target - mPlayer->pos();

  // 计算到目标的距离。
  double dist = toTarget.length();

  if (dist > 0)
  {
    // 枚举提供粗略减速等级，此系数进一步调整减速强度。
    const double decelerationTweaker = 0.3;

    // 根据距离和减速等级计算期望速度。
    double speed =  dist / ((double)deceleration * decelerationTweaker);

    // 限制期望速度不超过最大速度。
    speed = std::min(speed, mPlayer->maxSpeed());

    // 用已经算出的距离归一化目标方向，避免重复求长度。
    Vector2D desiredVelocity =  toTarget * speed / dist;

    return (desiredVelocity - mPlayer->velocity());
  }

  return Vector2D(0,0);
}


// 追踪行为：预测足球未来位置，再向预测位置移动。
Vector2D SteeringBehaviors::pursuit(const SoccerBall* ball)
{
  Vector2D toBall = ball->pos() - mPlayer->pos();

  // 预测时间由足球距离和速度共同决定。
  double lookAheadTime = 0.0;

  if (ball->speed() != 0.0)
  {
    lookAheadTime = toBall.length() / ball->speed();
  }

  // 估算足球在预测时间后的坐标。
  mTarget = ball->futurePosition(lookAheadTime);

  // 朝预测位置执行寻找行为。
  return arrive(mTarget, fast);
}


// 标记感知半径内的其他球员。
void SteeringBehaviors::findNeighbours()
{
  std::list<EntityPlayer*>& allPlayers = AutoList<EntityPlayer>::getAllMembers();
  std::list<EntityPlayer*>::iterator curPlyr;
  for (curPlyr = allPlayers.begin(); curPlyr!=allPlayers.end(); ++curPlyr)
  {
    // 先清除上一次的邻居标记。
    (*curPlyr)->steering()->unTag();

    // 比较距离平方以避免开平方。
    Vector2D to = (*curPlyr)->pos() - mPlayer->pos();

    if (to.lengthSq() < (mViewDistance * mViewDistance))
    {
      (*curPlyr)->steering()->tag();
    }
  }// 处理下一个球员。
}


// 分离行为：受到邻居的排斥力，减少球员聚集和重叠。
Vector2D SteeringBehaviors::separation()
{
   // 遍历邻居，计算从邻居指向当前球员的向量。
  Vector2D steeringForce;

  std::list<EntityPlayer*>& allPlayers = AutoList<EntityPlayer>::getAllMembers();
  std::list<EntityPlayer*>::iterator curPlyr;
  for (curPlyr = allPlayers.begin(); curPlyr!=allPlayers.end(); ++curPlyr)
  {
    // 排除自身，只考虑已标记为附近的对象。
    if((*curPlyr != mPlayer) && (*curPlyr)->steering()->tagged())
    {
      Vector2D toAgent = mPlayer->pos() - (*curPlyr)->pos();

      // 距离越近，排斥作用越强。
      steeringForce += vec2DNormalize(toAgent)/toAgent.length();
    }
  }

  return steeringForce;
}


// 阻挡行为：在球门侧目标和足球之间选择站位并减速到达。
Vector2D SteeringBehaviors::interpose(const SoccerBall* ball,
                                      Vector2D  target,
                                      double     distFromTarget)
{
  return arrive(target + vec2DNormalize(ball->pos() - target) *
                distFromTarget, normal);
}


// 绘制移动行为调试辅助图形。
void SteeringBehaviors::renderAids( )
{
  // 绘制移动力向量。
  gdi->redPen();

  gdi->line(mPlayer->pos(), mPlayer->pos() + mSteeringForce * 20);



}


