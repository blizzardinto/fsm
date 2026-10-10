/*
 * 阅读提示：场上球员具体类，继承球员公共能力，并拥有 FieldPlayerAI 控制器。
 * 继承用于表达它是一个球员；组合用于表达它有一个决策控制器，二者承担不同职责。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "EntityPlayerOnField.h"
#include "EntityPlayer.h"
#include "SteeringBehaviors.h"
#include "Transformations.h"
#include "Geometry.h"
#include "Cgdi.h"
#include "C2DMatrix.h"
#include "Goal.h"
#include "Region.h"
#include "EntityFunctionTemplates.h"
#include "ParamLoader.h"
#include "SoccerTeam.h"
#include "Regulator.h"
#include "DebugConsole.h"


#include <limits>

using std::vector;

// 析构函数释放球员拥有的踢球频率调节器；智能指针成员会自动销毁。
EntityPlayerOnField::~EntityPlayerOnField()
{
  delete mKickLimiter;
}

// 场上球员构造函数：先建立动作限制器，再初始化 AI 状态。
EntityPlayerOnField::EntityPlayerOnField(SoccerTeam* homeTeam,
                      int   homeRegion,
                      FieldPlayerState startState,
                      Vector2D  heading,
                      Vector2D velocity,
                      double    mass,
                      double    maxForce,
                      double    maxSpeed,
                      double    maxTurnRate,
                      double    scale,
                      PlayerRole role): EntityPlayer(homeTeam,
                                                    homeRegion,
                                                    heading,
                                                    velocity,
                                                    mass,
                                                    maxForce,
                                                    maxSpeed,
                                                    maxTurnRate,
                                                    scale,
                                                    role)
{
  mKickLimiter = new Regulator(prm.playerKickFrequency);
  mAi = std::make_unique<FieldPlayerAI>(*this);
  mAi->initialize(startState);
  mSteering->separationOn();
}

// 场上球员每次更新：决策、计算转向与加速、移动、处理重叠。
void EntityPlayerOnField::update()
{
  // 让 AI 控制器执行当前状态的决策。
  mAi->update();

  // 汇总移动行为产生的力。
  mSteering->calculate();

  // 没有移动力时逐步减速，模拟制动。
  if (mSteering->force().isZero())
  {
    const double brakingRate = 0.8;

    mVelocity = mVelocity * brakingRate;
  }

  // 侧向力用于转向；把本次转角限制在允许范围内。
  double turningForce =   mSteering->sideComponent();

  clamp(turningForce, -prm.playerMaxTurnRate, prm.playerMaxTurnRate);

  // 旋转球员的朝向向量。
  vec2DRotateAroundOrigin(mHeading, turningForce);

  // 让速度方向与新的朝向一致，同时保留速度大小。
  mVelocity = mHeading * mVelocity.length();

  // 重新计算与朝向垂直的侧向量。
  mSide = mHeading.perp();


  // 前向力除以质量得到前向加速度，再沿朝向更新速度。
  Vector2D accel = mHeading * mSteering->forwardComponent() / mMass;

  mVelocity += accel;

  // 限制速度大小。
  mVelocity.truncate(mMaxSpeed);

  // 根据速度更新位置。
  mPosition += mVelocity;


  // 配置启用时，推开与其他球员重叠的部分。
  if(prm.bNonPenetrationConstraint)
  {
    enforceNonPenetrationContraint(this, AutoList<EntityPlayer>::getAllMembers());
  }
}

// 将消息转交给场上球员的 AI 控制器。
bool EntityPlayerOnField::handleMessage(const Telegram& msg)
{
  return mAi->handleMessage(msg);
}

// 绘制球员及可选的调试信息。
void EntityPlayerOnField::render()
{
  gdi->transparentText();
  gdi->textColor(Cgdi::grey);

  // 根据所属球队选择颜色。
  if (team()->color() == SoccerTeam::blue){gdi->bluePen();}
  else{gdi->redPen();}



  // 绘制变换后的身体轮廓。
  mTransformedPlayerVertices = worldTransform(mPlayerVertices,
                                         pos(),
                                         heading(),
                                         side(),
                                         scale());
  gdi->closedShape(mTransformedPlayerVertices);

  // 绘制头部。
  gdi->brownBrush();
  if (prm.bHighlightIfThreatened && (team()->controllingPlayer() == this) && isThreatened()) gdi->yellowBrush();
  gdi->circle(pos(), 6);


  // 显示当前状态名称。
  if (prm.bStates)
  {
    gdi->textColor(0, 170, 0);
    gdi->textAtPos(mPosition.x, mPosition.y -20, std::string(mAi->getNameOfCurrentState()));
  }

  // 显示实体编号。
  if (prm.bIds)
  {
    gdi->textColor(0, 170, 0);
    gdi->textAtPos(pos().x-20, pos().y-20, ttos(id()));
  }


  if (prm.bViewTargets)
  {
    gdi->redBrush();
    gdi->circle(steering()->target(), 3);
    gdi->textAtPos(steering()->target(), ttos(id()));
  }
}




FieldPlayerAI* EntityPlayerOnField::getAi()const { return mAi.get(); }
bool EntityPlayerOnField::isReadyForNextKick()const { return mKickLimiter->isReady(); }
