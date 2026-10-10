/*
 * 阅读提示：通用状态调度器，只认识 State 接口，不认识足球、球员或球队。
 * 它调用进入、执行、退出和消息虚函数；真正的行为由当前状态对象决定，这就是运行时多态。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

class State;
struct Telegram;

// 状态机只借用状态指针；控制器拥有状态机和所有具体状态对象。
class StateMachine
{
public:
  StateMachine();
  StateMachine(const StateMachine&) = delete;
  StateMachine& operator=(const StateMachine&) = delete;

  // 初始状态必须存在；全局状态可以为空。开关用于决定是否立即调用初始进入动作。
  void initialize(State& initialState, State* globalState = nullptr,
                  bool enterInitialState = true);
  // 先执行全局状态，再执行当前状态。全局状态适合公共消息和持续检查。
  void update();
  bool handleMessage(const Telegram& message);
  // 引用参数不能为空。正常切换会保存旧状态，退出旧状态，再进入新状态。
  void changeState(State& nextState);
  void revertToPreviousState();

  bool isInState(const State& state) const;
  const State* currentState() const;
  const State* previousState() const;
  const State* globalState() const;
  const char* getNameOfCurrentState() const;

private:
  // 三个指针只是角色记录，并不拥有状态；调用方必须确保指向的对象仍然存在。
  State* mCurrentState;
  State* mPreviousState;
  State* mGlobalState;
};

#endif
