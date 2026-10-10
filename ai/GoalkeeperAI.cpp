/*
 * 阅读提示：守门员的决策控制器，封装守门、拦截、返回和发球状态。
 * 外部使用守门员专属枚举切换状态，不直接接触内部状态对象。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "GoalkeeperAI.h"
#include "StatesPlayerGoalKeeper.h"
#include "StateMachine.h"
#include <stdexcept>

struct GoalkeeperAI::Impl
{
  GlobalKeeperState mGlobalKeeperState;
  TendGoal mTendGoal;
  InterceptBall mInterceptBall;
  ReturnHome mReturnHome;
  PutBallBackInPlay mPutBallBackInPlay;
  // 成员按声明的逆序销毁：把状态机放最后，使它先于借用的状态对象销毁。
  StateMachine machine;

  // 每个状态成员都属于本控制器，构造时绑定本控制器服务的守门员。
  explicit Impl(EntityPlayerGoalKeeper& owner)
    : mGlobalKeeperState(owner),
      mTendGoal(owner),
      mInterceptBall(owner),
      mReturnHome(owner),
      mPutBallBackInPlay(owner)
  {}

  // 返回已有状态的引用，不创建临时状态；不同枚举对应不同的长期成员对象。
  State& select(GoalkeeperState id)
  {
    switch (id)
    {
    case GoalkeeperState::tendGoal: return mTendGoal;
    case GoalkeeperState::interceptBall: return mInterceptBall;
    case GoalkeeperState::returnHome: return mReturnHome;
    case GoalkeeperState::putBallBackInPlay: return mPutBallBackInPlay;
    }
    throw std::invalid_argument("Unknown GoalkeeperState");
  }
};

GoalkeeperAI::GoalkeeperAI(EntityPlayerGoalKeeper& owner)
  : mImpl(std::make_unique<Impl>(owner))
{}

GoalkeeperAI::~GoalkeeperAI() = default;

void GoalkeeperAI::initialize(GoalkeeperState initialState)
{
  mImpl->machine.initialize(mImpl->select(initialState), &mImpl->mGlobalKeeperState, true);
}

void GoalkeeperAI::update() { mImpl->machine.update(); }
bool GoalkeeperAI::handleMessage(const Telegram& message)
{
  return mImpl->machine.handleMessage(message);
}
void GoalkeeperAI::changeState(GoalkeeperState nextState)
{
  mImpl->machine.changeState(mImpl->select(nextState));
}
void GoalkeeperAI::revertToPreviousState() { mImpl->machine.revertToPreviousState(); }
bool GoalkeeperAI::isInState(GoalkeeperState state) const
{
  return mImpl->machine.isInState(mImpl->select(state));
}
const char* GoalkeeperAI::getNameOfCurrentState() const
{
  return mImpl->machine.getNameOfCurrentState();
}
