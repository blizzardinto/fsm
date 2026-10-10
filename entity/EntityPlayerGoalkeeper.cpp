/*
 * 阅读提示：守门员具体类，继承球员公共能力，并拥有 GoalkeeperAI 控制器。
 * 球队通过 EntityPlayer 指针统一更新成员，虚函数会自动选择守门员的实现。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "EntityPlayerGoalKeeper.h"
#include "Cgdi.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "transformations.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "EntityFunctionTemplates.h"
#include "ParamLoader.h"



// 守门员构造函数：初始化角色及专属 AI 控制器。
EntityPlayerGoalKeeper::EntityPlayerGoalKeeper(SoccerTeam*        homeTeam,
                       int                homeRegion,
                       GoalkeeperState startState,
                       Vector2D           heading,
                       Vector2D           velocity,
                       double              mass,
                       double              maxForce,
                       double              maxSpeed,
                       double              maxTurnRate,
                       double              scale): EntityPlayer(homeTeam,
                                                             homeRegion,
                                                             heading,
                                                             velocity,
                                                             mass,
                                                             maxForce,
                                                             maxSpeed,
                                                             maxTurnRate,
                                                             scale,
                                                             EntityPlayer::goalKeeper)


{
  mAi = std::make_unique<GoalkeeperAI>(*this);
  mAi->initialize(startState);
}



// 每次更新先作决策，再根据移动力更新速度和位置。

void EntityPlayerGoalKeeper::update()
{
  // 让 AI 控制器执行当前状态的决策。
  mAi->update();

  // 汇总已启用移动行为产生的力。
  Vector2D steeringForce = mSteering->calculate();



  // 根据牛顿第二定律计算加速度：加速度等于力除以质量。
  Vector2D acceleration = steeringForce / mMass;

  // 把加速度累加到速度。
  mVelocity += acceleration;

  // 限制速度大小，避免超过最大速度。
  mVelocity.truncate(mMaxSpeed);

  // 根据速度更新位置。
  mPosition += mVelocity;


  // 配置启用时，处理球员之间的重叠。
  if(prm.bNonPenetrationConstraint)
  {
    enforceNonPenetrationContraint(this, AutoList<EntityPlayer>::getAllMembers());
  }

  // 速度非零时，更新前进方向和侧向量。
  if ( !mVelocity.isZero())
  {
    mHeading = vec2DNormalize(mVelocity);

    mSide = mHeading.perp();
  }

  // 绘图用的观察方向始终指向足球，不一定等于移动方向。
  if (!pitch()->entityPlayerGoalKeeperHasBall())
  {
   mLookAt = vec2DNormalize(ball()->pos() - pos());
  }
}


bool EntityPlayerGoalKeeper::ballWithinRangeForIntercept()const
{
  return (vec2DDistanceSq(team()->homeGoal()->center(), ball()->pos()) <=
          prm.entityPlayerGoalKeeperInterceptRangeSq);
}

bool EntityPlayerGoalKeeper::tooFarFromGoalMouth()const
{
  return (vec2DDistanceSq(pos(), getRearInterposeTarget()) >
          prm.entityPlayerGoalKeeperInterceptRangeSq);
}

Vector2D EntityPlayerGoalKeeper::getRearInterposeTarget()const
{
  double xPosTarget = team()->homeGoal()->center().x;

  double yPosTarget = pitch()->playingArea()->center().y -
                     prm.goalWidth*0.5 + (ball()->pos().y*prm.goalWidth) /
                     pitch()->playingArea()->height();

  return Vector2D(xPosTarget, yPosTarget);
}

// 把收到的消息转交给 AI 控制器处理。
bool EntityPlayerGoalKeeper::handleMessage(const Telegram& msg)
{
  return mAi->handleMessage(msg);
}

// 绘制守门员及可选调试信息。
void EntityPlayerGoalKeeper::render()
{
  if (team()->color() == SoccerTeam::blue)
    gdi->bluePen();
  else
    gdi->redPen();

  mTransformedPlayerVertices = worldTransform(mPlayerVertices,
                                       pos(),
                                       mLookAt,
                                       mLookAt.perp(),
                                       scale());

  gdi->closedShape(mTransformedPlayerVertices);

  // 绘制头部。
  gdi->brownBrush();
  gdi->circle(pos(), 6);

  // 绘制实体编号。
  if (prm.bIds)
  {
    gdi->textColor(0, 170, 0);;
    gdi->textAtPos(pos().x-20, pos().y-20, ttos(id()));
  }

  // 绘制当前状态名称。
  if (prm.bStates)
  {
    gdi->textColor(0, 170, 0);
    gdi->transparentText();
    gdi->textAtPos(mPosition.x, mPosition.y -20, std::string(mAi->getNameOfCurrentState()));
  }
}

EntityPlayerGoalKeeper::~EntityPlayerGoalKeeper() = default;
GoalkeeperAI* EntityPlayerGoalKeeper::getAi()const { return mAi.get(); }
