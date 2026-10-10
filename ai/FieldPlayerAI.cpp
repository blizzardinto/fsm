/*
 * 阅读提示：场上球员的决策控制器，将球员可用的状态和调度器封装在一起。
 * 每名球员独占一份控制器，因此不同球员可以同时处于不同状态。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "FieldPlayerAI.h"
#include "StatesPlayerOnField.h"
#include "StateMachine.h"
#include <stdexcept>

struct FieldPlayerAI::Impl
{
  GlobalPlayerState mGlobalPlayerState;
  ChaseBall mChaseBall;
  Dribble mDribble;
  ReturnToHomeRegion mReturnToHomeRegion;
  Wait mWait;
  KickBall mKickBall;
  ReceiveBall mReceiveBall;
  SupportAttacker mSupportAttacker;
  // 成员按声明的逆序销毁：把状态机放最后，使它先于借用的状态对象销毁。
  StateMachine machine;

  // 初始化列表按成员声明顺序构造各状态，并把同一个球员引用绑定到每个状态。
  explicit Impl(EntityPlayerOnField& owner)
    : mGlobalPlayerState(owner),
      mChaseBall(owner),
      mDribble(owner),
      mReturnToHomeRegion(owner),
      mWait(owner),
      mKickBall(owner),
      mReceiveBall(owner),
      mSupportAttacker(owner)
  {}

  // 返回已有状态的引用，不创建临时状态；不同枚举对应不同的长期成员对象。
  State& select(FieldPlayerState id)
  {
    switch (id)
    {
    case FieldPlayerState::chaseBall: return mChaseBall;
    case FieldPlayerState::dribble: return mDribble;
    case FieldPlayerState::returnToHomeRegion: return mReturnToHomeRegion;
    case FieldPlayerState::wait: return mWait;
    case FieldPlayerState::kickBall: return mKickBall;
    case FieldPlayerState::receiveBall: return mReceiveBall;
    case FieldPlayerState::supportAttacker: return mSupportAttacker;
    }
    throw std::invalid_argument("Unknown FieldPlayerState");
  }
};

FieldPlayerAI::FieldPlayerAI(EntityPlayerOnField& owner)
  : mImpl(std::make_unique<Impl>(owner))
{}

FieldPlayerAI::~FieldPlayerAI() = default;

void FieldPlayerAI::initialize(FieldPlayerState initialState)
{
  mImpl->machine.initialize(mImpl->select(initialState), &mImpl->mGlobalPlayerState, true);
}

void FieldPlayerAI::update() { mImpl->machine.update(); }
bool FieldPlayerAI::handleMessage(const Telegram& message)
{
  return mImpl->machine.handleMessage(message);
}
void FieldPlayerAI::changeState(FieldPlayerState nextState)
{
  mImpl->machine.changeState(mImpl->select(nextState));
}
void FieldPlayerAI::revertToPreviousState() { mImpl->machine.revertToPreviousState(); }
bool FieldPlayerAI::isInState(FieldPlayerState state) const
{
  return mImpl->machine.isInState(mImpl->select(state));
}
const char* FieldPlayerAI::getNameOfCurrentState() const
{
  return mImpl->machine.getNameOfCurrentState();
}
