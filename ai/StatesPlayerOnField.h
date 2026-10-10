/*
 * 阅读提示：场上球员状态集合，分别处理追球、踢球、等待、带球、接球和支援。
 * 状态决定做什么，移动行为负责怎么算移动力，球员更新负责真正改变速度和位置。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef STATESPLAYERONFIELD_H
#define STATESPLAYERONFIELD_H

#include "State.h"

class EntityPlayerOnField;

// 场上球员的公共检查和消息处理：继承统一接口，但提供这一动作自己的实现。
class GlobalPlayerState : public State
{
public:
  explicit GlobalPlayerState(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 追赶足球：继承统一接口，但提供这一动作自己的实现。
class ChaseBall : public State
{
public:
  explicit ChaseBall(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 短距离踢球推进：继承统一接口，但提供这一动作自己的实现。
class Dribble : public State
{
public:
  explicit Dribble(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 返回分配的区域：继承统一接口，但提供这一动作自己的实现。
class ReturnToHomeRegion : public State
{
public:
  explicit ReturnToHomeRegion(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 等待并保持站位：继承统一接口，但提供这一动作自己的实现。
class Wait : public State
{
public:
  explicit Wait(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 选择射门、传球或带球：继承统一接口，但提供这一动作自己的实现。
class KickBall : public State
{
public:
  explicit KickBall(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 移动接球：继承统一接口，但提供这一动作自己的实现。
class ReceiveBall : public State
{
public:
  explicit ReceiveBall(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

// 支援控球队员：继承统一接口，但提供这一动作自己的实现。
class SupportAttacker : public State
{
public:
  explicit SupportAttacker(EntityPlayerOnField& owner);
  void enter() override;
  void execute() override;
  void exit() override;
  bool onMessage(const Telegram& telegram) override;
  const char* name() const override;

private:
  // 借用所属对象的引用，不复制也不销毁它；所属对象通过控制器拥有这个状态。
  EntityPlayerOnField& mOwner;
};

#endif
