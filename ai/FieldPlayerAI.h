/*
 * 阅读提示：场上球员的决策控制器，将球员可用的状态和调度器封装在一起。
 * 每名球员独占一份控制器，因此不同球员可以同时处于不同状态。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef FIELDPLAYERAI_H
#define FIELDPLAYERAI_H

#include <memory>

class EntityPlayerOnField;
struct Telegram;

// 枚举限定此角色能够使用的状态，编译器可以阻止把另一角色的枚举传进来。
enum class FieldPlayerState
{
  chaseBall,
  dribble,
  returnToHomeRegion,
  wait,
  kickBall,
  receiveBall,
  supportAttacker
};

// 控制器把角色专属枚举转换成自己的状态对象；外部无需认识具体状态类。
class FieldPlayerAI
{
public:
  // 引用绑定已有业务对象，不复制它。explicit 防止对象被意外地隐式转换成控制器。
  explicit FieldPlayerAI(EntityPlayerOnField& owner);
  ~FieldPlayerAI();
  FieldPlayerAI(const FieldPlayerAI&) = delete;
  FieldPlayerAI& operator=(const FieldPlayerAI&) = delete;

  void initialize(FieldPlayerState initialState);
  void update();
  bool handleMessage(const Telegram& message);
  void changeState(FieldPlayerState nextState);
  void revertToPreviousState();
  bool isInState(FieldPlayerState state) const;
  const char* getNameOfCurrentState() const;

private:
  // 只声明内部实现类型。具体状态成员放在实现文件，使用者无需包含全部状态头文件。
  struct Impl;
  // 独占智能指针负责销毁内部实现；析构放在源文件，因为那里才能看到完整的实现类型。
  std::unique_ptr<Impl> mImpl;
};

#endif
