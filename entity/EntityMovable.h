/*
 * 阅读提示：可移动实体基类，在 EntityBase 之上复用速度、质量、朝向和转向能力。
 * 继承表示一种关系：球员和足球都是可移动实体，公共运动数据不必在每个派生类重复定义。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef MOVING_ENTITY
#define MOVING_ENTITY
// 可移动实体基类：在实体身份和位置之上增加速度、质量和局部坐标轴。原作者：Mat Buckland（fup@ai-junkie.com）。

#include <cassert>

#include "Vector2D.h"
#include "EntityBase.h"



// public 继承允许外部把可移动实体当作基础实体使用，复用它的身份和位置接口。
class EntityMovable : public EntityBase
{
protected:

  Vector2D    mVelocity;

  // 指向前进方向的单位向量。
  Vector2D    mHeading;

  // 与前进方向垂直的侧向单位向量。
  Vector2D    mSide;

  double      mMass;

  // 实体允许的最大移动速度。
  double      mMaxSpeed;

  // 实体移动时允许施加的最大驱动力。
  double      mMaxForce;

  // 每次转向计算允许的最大转角；具体时间尺度由调用方式决定。
  double      mMaxTurnRate;

public:


  EntityMovable(Vector2D position,
               double   radius,
               Vector2D velocity,
               double   maxSpeed,
               Vector2D heading,
               double   mass,
               Vector2D scale,
               double   turnRate,
               double   maxForce):EntityBase(EntityBase::getNextValidId()),
                                  mHeading(heading),
                                  mVelocity(velocity),
                                  mMass(mass),
                                  mSide(mHeading.perp()),
                                  mMaxSpeed(maxSpeed),
                                  mMaxTurnRate(turnRate),
                                  mMaxForce(maxForce)
  {
    mPosition = position;
    mBoundingRadius = radius;
    mScale = scale;
  }


  virtual ~EntityMovable(){}

  // 访问器提供读取和修改成员的接口。
  Vector2D  velocity()const{return mVelocity;}
  void      setVelocity(const Vector2D& newVel){mVelocity = newVel;}

  double    mass()const{return mMass;}

  Vector2D  side()const{return mSide;}

  double    maxSpeed()const{return mMaxSpeed;}
  void      setMaxSpeed(double newSpeed){mMaxSpeed = newSpeed;}

  double    maxForce()const{return mMaxForce;}
  void      setMaxForce(double mf){mMaxForce = mf;}

  bool      isSpeedMaxedOut()const{return mMaxSpeed*mMaxSpeed >= mVelocity.lengthSq();}
  double    speed()const{return mVelocity.length();}
  double    speedSq()const{return mVelocity.lengthSq();}

  Vector2D  heading()const{return mHeading;}
  void      setHeading(Vector2D newHeading);
  bool      rotateHeadingToFacePosition(Vector2D target);

  double    maxTurnRate()const{return mMaxTurnRate;}
  void      setMaxTurnRate(double val){mMaxTurnRate = val;}

};


// 朝目标位置转向，每次旋转不超过最大转角；已经面对目标时返回真。
inline bool EntityMovable::rotateHeadingToFacePosition(Vector2D target)
{
  Vector2D toTarget = vec2DNormalize(target - mPosition);

  double dot = mHeading.dot(toTarget);

  // 把点积限制在反余弦函数的有效区间，防止浮点误差产生无效结果。
  clamp(dot, -1, 1);

  // 计算当前朝向与目标方向的夹角。
  double angle = acos(dot);

  // 已经面向目标时，无需继续旋转。
  if (angle < 0.00001) return true;

  // 把本次转角限制在允许的最大值以内。
  if (angle > mMaxTurnRate) angle = mMaxTurnRate;

  // 用旋转矩阵同时更新朝向和速度。
  C2DMatrix rotationMatrix;

  // 根据叉积符号决定旋转方向。
  rotationMatrix.rotate(angle * mHeading.sign(toTarget));
  rotationMatrix.transformVector2Ds(mHeading);
  rotationMatrix.transformVector2Ds(mVelocity);

  // 从新的朝向重新计算垂直的侧向量。
  mSide = mHeading.perp();

  return false;
}


// 设置朝向，并同步计算侧向量；传入方向需要满足实现中的有效性检查。
inline void EntityMovable::setHeading(Vector2D newHeading)
{
  assert( (newHeading.lengthSq() - 1.0) < 0.00001);

  mHeading = newHeading;

  // 侧向量始终与朝向垂直。
  mSide = mHeading.perp();
}




#endif