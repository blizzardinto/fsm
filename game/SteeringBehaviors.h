/*
 * 阅读提示：移动行为组件，借用一个球员对象，并把目标与行为开关换算成移动力。
 * 组合允许球员复用寻找、到达、追踪、分离和阻挡算法；此组件不负责选择足球战术状态。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef SteeringBehaviorsS_H
#define SteeringBehaviorsS_H
#pragma warning (disable:4786)
#include <vector>
#include <windows.h>
#include <string>


#include "Vector2D.h"

class EntityPlayer;
class SoccerPitch;
class SoccerBall;
class CWall;
class CObstacle;


// 移动行为类的成员和内部算法。

class SteeringBehaviors
{
private:

  EntityPlayer*   mPlayer;

  SoccerBall*   mBall;

  // 当前已启用行为共同产生的移动力。
  Vector2D     mSteeringForce;

  // 当前目标，通常是足球或预测的足球位置。
  Vector2D     mTarget;

  // 阻挡行为中与球门侧目标保持的距离。
  double        mInterposeDist;

  // 各行为的权重，用来调整其影响大小。
  double        mMultSeparation;

  // 感知邻居的距离。
  double        mViewDistance;


  // 用不同二进制位表示哪些行为被启用；多个行为可同时开启。
  int           mFlags;

  enum BehaviorType
  {
    none               = 0x0000,
    seekBehavior               = 0x0001,
    arriveBehavior             = 0x0002,
    separationBehavior         = 0x0004,
    pursuitBehavior            = 0x0008,
    interposeBehavior          = 0x0010
  };

  // 邻居标记使用的内部编号。
  bool         mTagged;

  // 到达行为的减速等级。
  enum Deceleration{slow = 3, normal = 2, fast = 1};


  // 朝目标移动的寻找行为。
  Vector2D seek(Vector2D target);

  // 接近目标时减速的到达行为。
  Vector2D arrive(Vector2D target, Deceleration decel);

  // 预测足球位置并向预测点移动的追踪行为。
  Vector2D pursuit(const SoccerBall* ball);

  Vector2D separation();

  // 移动到球门侧目标与足球之间的阻挡行为。
  Vector2D interpose(const SoccerBall* ball,
                     Vector2D pos,
                     double    distFromTarget);


  // 标记感知半径内的邻居。
  void      findNeighbours();


  // 检查行为标志中的某一位是否已设置。
  bool      on(BehaviorType bt){return (mFlags & bt) == bt;}

  bool      accumulateForce(Vector2D &sf, Vector2D forceToAdd);

  Vector2D  sumForces();

  // 为探测方向保留的顶点缓冲区。
  std::vector<Vector2D> mAntenna;


public:

  SteeringBehaviors(EntityPlayer*       agent,
                    SoccerPitch*  world,
                    SoccerBall*   ball);

  virtual ~SteeringBehaviors(){}


  Vector2D calculate();

  // 计算与朝向平行的力分量。
  double    forwardComponent();

  // 计算与朝向垂直的力分量。
  double    sideComponent();

  Vector2D force()const{return mSteeringForce;}

  // 绘制目标和移动力，帮助观察行为计算结果。
  void      renderInfo();
  void      renderAids();

  Vector2D  target()const{return mTarget;}
  void      setTarget(const Vector2D t){mTarget = t;}

  double     interposeDistance()const{return mInterposeDist;}
  void      setInterposeDistance(double d){mInterposeDist = d;}

  bool      tagged()const{return mTagged;}
  void      tag(){mTagged = true;}
  void      unTag(){mTagged = false;}


  void seekOn(){mFlags |= seekBehavior;}
  void arriveOn(){mFlags |= arriveBehavior;}
  void pursuitOn(){mFlags |= pursuitBehavior;}
  void separationOn(){mFlags |= separationBehavior;}
  void interposeOn(double d){mFlags |= interposeBehavior; mInterposeDist = d;}


  void seekOff()  {if(on(seekBehavior))   mFlags ^=seekBehavior;}
  void arriveOff(){if(on(arriveBehavior)) mFlags ^=arriveBehavior;}
  void pursuitOff(){if(on(pursuitBehavior)) mFlags ^=pursuitBehavior;}
  void separationOff(){if(on(separationBehavior)) mFlags ^=separationBehavior;}
  void interposeOff(){if(on(interposeBehavior)) mFlags ^=interposeBehavior;}


  bool seekIsOn(){return on(seekBehavior);}
  bool arriveIsOn(){return on(arriveBehavior);}
  bool pursuitIsOn(){return on(pursuitBehavior);}
  bool separationIsOn(){return on(separationBehavior);}
  bool interposeIsOn(){return on(interposeBehavior);}

};




#endif