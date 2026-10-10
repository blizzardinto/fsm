/*
 * 阅读提示：球队的决策控制器，负责进攻、防守和开球准备策略。
 * 它与球员控制器职责不同：球队决定整体安排，球员决定自己的具体动作。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "TeamAI.h"
#include "StatesTeam.h"
#include "StateMachine.h"
#include <stdexcept>

struct TeamAI::Impl
{
  Attacking mAttacking;
  Defending mDefending;
  PrepareForKickOff mPrepareForKickOff;
  // 成员按声明的逆序销毁：把状态机放最后，使它先于借用的状态对象销毁。
  StateMachine machine;

  // 状态对象作为成员直接构造，不通过全局单例共享，因此各球队的状态彼此独立。
  explicit Impl(SoccerTeam& owner)
    : mAttacking(owner),
      mDefending(owner),
      mPrepareForKickOff(owner)
  {}

  // 返回已有状态的引用，不创建临时状态；不同枚举对应不同的长期成员对象。
  State& select(TeamState id)
  {
    switch (id)
    {
    case TeamState::attacking: return mAttacking;
    case TeamState::defending: return mDefending;
    case TeamState::prepareForKickOff: return mPrepareForKickOff;
    }
    throw std::invalid_argument("Unknown TeamState");
  }
};

TeamAI::TeamAI(SoccerTeam& owner)
  : mImpl(std::make_unique<Impl>(owner))
{}

TeamAI::~TeamAI() = default;

void TeamAI::initialize(TeamState initialState)
{
  mImpl->machine.initialize(mImpl->select(initialState), nullptr, false);
}

void TeamAI::update() { mImpl->machine.update(); }
bool TeamAI::handleMessage(const Telegram& message)
{
  return mImpl->machine.handleMessage(message);
}
void TeamAI::changeState(TeamState nextState)
{
  mImpl->machine.changeState(mImpl->select(nextState));
}
void TeamAI::revertToPreviousState() { mImpl->machine.revertToPreviousState(); }
bool TeamAI::isInState(TeamState state) const
{
  return mImpl->machine.isInState(mImpl->select(state));
}
const char* TeamAI::getNameOfCurrentState() const
{
  return mImpl->machine.getNameOfCurrentState();
}
