/*
 * 阅读提示：球队状态集合。每个类代表一套球队策略，并在构造时绑定同一个所属球队。
 * 把变化的策略拆成状态类后，球队不需要在更新函数里写一个庞大的条件分支。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef STATESTEAM_H
#define STATESTEAM_H

#include "State.h"

class SoccerTeam;

// 进攻阵型和支援安排：继承统一接口，但提供这一动作自己的实现。
class Attacking : public State
{
public:
  explicit Attacking(SoccerTeam& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  SoccerTeam& mOwner;
};

// 防守阵型安排：继承统一接口，但提供这一动作自己的实现。
class Defending : public State
{
public:
  explicit Defending(SoccerTeam& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  SoccerTeam& mOwner;
};

// 开球前恢复双方站位：继承统一接口，但提供这一动作自己的实现。
class PrepareForKickOff : public State
{
public:
  explicit PrepareForKickOff(SoccerTeam& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  SoccerTeam& mOwner;
};

#endif
