/*
 * 阅读提示：守门员状态集合，分别处理守门站位、出击拦截、返回和重新发球。
 * 各状态借用所属守门员，通过统一的 State 接口接受状态机调用。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef STATESPLAYERGOALKEEPER_H
#define STATESPLAYERGOALKEEPER_H

#include "State.h"

class EntityPlayerGoalKeeper;

// 守门员的公共消息处理：继承统一接口，但提供这一动作自己的实现。
class GlobalKeeperState : public State
{
public:
  explicit GlobalKeeperState(EntityPlayerGoalKeeper& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerGoalKeeper& mOwner;
};

// 在门前调整站位：继承统一接口，但提供这一动作自己的实现。
class TendGoal : public State
{
public:
  explicit TendGoal(EntityPlayerGoalKeeper& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerGoalKeeper& mOwner;
};

// 出击拦截足球：继承统一接口，但提供这一动作自己的实现。
class InterceptBall : public State
{
public:
  explicit InterceptBall(EntityPlayerGoalKeeper& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerGoalKeeper& mOwner;
};

// 返回守门区域：继承统一接口，但提供这一动作自己的实现。
class ReturnHome : public State
{
public:
  explicit ReturnHome(EntityPlayerGoalKeeper& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerGoalKeeper& mOwner;
};

// 寻找队友并重新发球：继承统一接口，但提供这一动作自己的实现。
class PutBallBackInPlay : public State
{
public:
  explicit PutBallBackInPlay(EntityPlayerGoalKeeper& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerGoalKeeper& mOwner;
};

#endif
