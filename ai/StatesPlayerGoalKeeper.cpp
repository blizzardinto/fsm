/*
 * 阅读提示：守门员状态集合，分别处理守门站位、出击拦截、返回和重新发球。
 * 各状态借用所属守门员，通过统一的 State 接口接受状态机调用。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "StatesPlayerGoalKeeper.h"
#include "DebugConsole.h"
#include "SoccerPitch.h"
#include "EntityPlayer.h"
#include "EntityPlayerGoalKeeper.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "geometry.h"
#include "EntityPlayerOnField.h"
#include "ParamLoader.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"


// 如需观察守门员状态，可启用调试宏 GOALY_STATE_INFO_ON。以下是全局状态的实现。




bool GlobalKeeperState::onMessage(const Telegram& telegram)
{
  // 这是所属守门员的借用指针，状态只调用它的接口，不接管它的生命周期。
  auto* keeper = &mOwner;
  switch(telegram.msg)
  {
    case msgGoHome:
    {
      keeper->setDefaultHomeRegion();

      keeper->getAi()->changeState(GoalkeeperState::returnHome);
    }

    break;

    case msgReceiveBall:
      {
        keeper->getAi()->changeState(GoalkeeperState::interceptBall);
      }

      break;

  }// 结束消息类型的分支处理。

  return false;
}


// 守门状态：通过阻挡行为站在足球与球门目标之间；满足出击条件后改为拦截状态。




void TendGoal::enter()
{
  auto* keeper = &mOwner;
  // 开启阻挡行为，让移动模块负责计算站位。
  keeper->steering()->interposeOn(prm.entityPlayerGoalKeeperTendingDistance);

  // 设置球门附近的目标，守门员将站在该目标与足球之间。
  keeper->steering()->setTarget(keeper->getRearInterposeTarget());
}

void TendGoal::execute()
{
  auto* keeper = &mOwner;
  // 足球位置会变化，因此每次更新都要刷新球门侧的阻挡目标。
  keeper->steering()->setTarget(keeper->getRearInterposeTarget());

  // 足球进入接球范围时停住球，再切换到重新发球状态。
  if (keeper->ballWithinKeeperRange())
  {
    keeper->ball()->trap();

    keeper->pitch()->setEntityPlayerGoalKeeperHasBall(true);

    keeper->getAi()->changeState(GoalkeeperState::putBallBackInPlay);

    return;
  }

  // 满足拦截距离条件时，离开门线去追球。
  if (keeper->ballWithinRangeForIntercept() && !keeper->team()->inControl())
  {
    keeper->getAi()->changeState(GoalkeeperState::interceptBall);
  }

  // 离球门太远且本队控球时，返回球门附近。
  if (keeper->tooFarFromGoalMouth() && keeper->team()->inControl())
  {
    keeper->getAi()->changeState(GoalkeeperState::returnHome);

    return;
  }
}


void TendGoal::exit()
{
  auto* keeper = &mOwner;
  keeper->steering()->interposeOff();
}


// 返回状态：移动到自己的区域，条件满足后恢复守门。




void ReturnHome::enter()
{
  auto* keeper = &mOwner;
  keeper->steering()->arriveOn();
}

void ReturnHome::execute()
{
  auto* keeper = &mOwner;
  keeper->steering()->setTarget(keeper->homeRegion()->center());

  // 接近本方区域，或对方重新控球时，恢复守门。
  if (keeper->inHomeRegion() || !keeper->team()->inControl())
  {
    keeper->getAi()->changeState(GoalkeeperState::tendGoal);
  }
}

void ReturnHome::exit()
{
  auto* keeper = &mOwner;
  keeper->steering()->arriveOff();
}



// 拦截状态：用追踪行为追赶足球，并根据距离与最近球员条件决定是否返回。




void InterceptBall::enter()
{
  auto* keeper = &mOwner;
  keeper->steering()->pursuitOn();

    #ifdef GOALY_STATE_INFO_ON
    debugCon << "Goaly " << keeper->id() << " enters InterceptBall" <<  "";
    #endif
}

void InterceptBall::execute()
{
  auto* keeper = &mOwner;
  // 如果离球门太远且自己不是最近的球员，就返回；否则继续拦截。
  if (keeper->tooFarFromGoalMouth() && !keeper->isClosestPlayerOnPitchToBall())
  {
    keeper->getAi()->changeState(GoalkeeperState::returnHome);

    return;
  }

  // 足球进入手的接球范围后停球，再准备发球。
  if (keeper->ballWithinKeeperRange())
  {
    keeper->ball()->trap();

    keeper->pitch()->setEntityPlayerGoalKeeperHasBall(true);

    keeper->getAi()->changeState(GoalkeeperState::putBallBackInPlay);

    return;
  }
}

void InterceptBall::exit()
{
  auto* keeper = &mOwner;
  keeper->steering()->pursuitOff();
}



// 重新发球状态：选择队友并把球传回比赛。



void PutBallBackInPlay::enter()
{
  auto* keeper = &mOwner;
  // 告诉球队当前由守门员控球。
  keeper->team()->setControllingPlayer(keeper);

  // 通知双方场上球员返回各自区域。
  keeper->team()->opponents()->returnAllEntityPlayerOnFieldsToHome();
  keeper->team()->returnAllEntityPlayerOnFieldsToHome();
}


void PutBallBackInPlay::execute()
{
  auto* keeper = &mOwner;
  EntityPlayer*  receiver = NULL;
  Vector2D     ballTarget;

  // 寻找能够接到传球的队友。
  if (keeper->team()->findPass(keeper,
                              receiver,
                              ballTarget,
                              prm.maxPassingForce,
                              prm.goalkeeperMinPassDist))
  {
    // 按目标方向和传球力量踢球。
    keeper->ball()->kick(vec2DNormalize(ballTarget - keeper->ball()->pos()),
                         prm.maxPassingForce);

    // 守门员已经发球，解除持球标记。
    keeper->pitch()->setEntityPlayerGoalKeeperHasBall(false);

    // 发送消息，告诉接球队员球正在传向他。
    dispatcher->dispatchMsg(sendMsgImmediately,
                          keeper->id(),
                          receiver->id(),
                          msgReceiveBall,
                          &ballTarget);

    // 发球完成，恢复守门状态。
    keeper->getAi()->changeState(GoalkeeperState::tendGoal);

    return;
  }

  keeper->setVelocity(Vector2D());
}


GlobalKeeperState::GlobalKeeperState(EntityPlayerGoalKeeper& owner) : mOwner(owner) {}

const char* GlobalKeeperState::name() const { return "GlobalKeeperState"; }

void GlobalKeeperState::enter() {}

void GlobalKeeperState::execute() {}

void GlobalKeeperState::exit() {}


TendGoal::TendGoal(EntityPlayerGoalKeeper& owner) : mOwner(owner) {}

const char* TendGoal::name() const { return "TendGoal"; }

bool TendGoal::onMessage(const Telegram& telegram) {return false;}


InterceptBall::InterceptBall(EntityPlayerGoalKeeper& owner) : mOwner(owner) {}

const char* InterceptBall::name() const { return "InterceptBall"; }

bool InterceptBall::onMessage(const Telegram& telegram) {return false;}


ReturnHome::ReturnHome(EntityPlayerGoalKeeper& owner) : mOwner(owner) {}

const char* ReturnHome::name() const { return "ReturnHome"; }

bool ReturnHome::onMessage(const Telegram& telegram) {return false;}


PutBallBackInPlay::PutBallBackInPlay(EntityPlayerGoalKeeper& owner) : mOwner(owner) {}

const char* PutBallBackInPlay::name() const { return "PutBallBackInPlay"; }

void PutBallBackInPlay::exit() {}

bool PutBallBackInPlay::onMessage(const Telegram& telegram) {return false;}

