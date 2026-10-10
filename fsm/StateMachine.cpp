/*
 * 阅读提示：通用状态调度器，只认识 State 接口，不认识足球、球员或球队。
 * 它调用进入、执行、退出和消息虚函数；真正的行为由当前状态对象决定，这就是运行时多态。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "StateMachine.h"
#include "State.h"

StateMachine::StateMachine()
  : mCurrentState(nullptr), mPreviousState(nullptr), mGlobalState(nullptr)
{}

void StateMachine::initialize(State& initialState, State* globalState,
                              bool enterInitialState)
{
  mCurrentState = &initialState;
  mPreviousState = &initialState;
  mGlobalState = globalState;
  if (enterInitialState) mCurrentState->enter();
}

void StateMachine::update()
{
  if (mGlobalState) mGlobalState->execute();
  // 全局状态可能切换当前状态，因此随后重新读取当前状态并执行。
  if (mCurrentState) mCurrentState->execute();
}

bool StateMachine::handleMessage(const Telegram& message)
{
  if (mCurrentState && mCurrentState->onMessage(message)) return true;
  if (mGlobalState && mGlobalState->onMessage(message)) return true;
  return false;
}

void StateMachine::changeState(State& nextState)
{
  // 保存上一个状态，供返回操作使用。这里改变的是调度关系，不是创建新状态。
  mPreviousState = mCurrentState;
  if (mCurrentState) mCurrentState->exit();
  mCurrentState = &nextState;
  mCurrentState->enter();
}

void StateMachine::revertToPreviousState()
{
  if (mPreviousState) changeState(*mPreviousState);
}

bool StateMachine::isInState(const State& state) const
{
  return mCurrentState == &state;
}

const State* StateMachine::currentState() const { return mCurrentState; }
const State* StateMachine::previousState() const { return mPreviousState; }
const State* StateMachine::globalState() const { return mGlobalState; }

const char* StateMachine::getNameOfCurrentState() const
{
  return mCurrentState ? mCurrentState->name() : "None";
}
