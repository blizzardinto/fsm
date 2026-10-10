/*
 * 阅读提示：守门员的决策控制器，封装守门、拦截、返回和发球状态。
 * 外部使用守门员专属枚举切换状态，不直接接触内部状态对象。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef GOALKEEPERAI_H
#define GOALKEEPERAI_H

#include <memory>

class EntityPlayerGoalKeeper;
struct Telegram;

// 枚举限定此角色能够使用的状态，编译器可以阻止把另一角色的枚举传进来。
enum class GoalkeeperState
{
  tendGoal,
  interceptBall,
  returnHome,
  putBallBackInPlay
};

// 控制器把角色专属枚举转换成自己的状态对象；外部无需认识具体状态类。
class GoalkeeperAI
{
public:
  // 引用绑定已有业务对象，不复制它。explicit 防止对象被意外地隐式转换成控制器。
  explicit GoalkeeperAI(EntityPlayerGoalKeeper& owner);
  ~GoalkeeperAI();
  GoalkeeperAI(const GoalkeeperAI&) = delete;
  GoalkeeperAI& operator=(const GoalkeeperAI&) = delete;

  void initialize(GoalkeeperState initialState);
  void update();
  bool handleMessage(const Telegram& message);
  void changeState(GoalkeeperState nextState);
  void revertToPreviousState();
  bool isInState(GoalkeeperState state) const;
  const char* getNameOfCurrentState() const;

private:
  // 只声明内部实现类型。具体状态成员放在实现文件，使用者无需包含全部状态头文件。
  struct Impl;
  // 独占智能指针负责销毁内部实现；析构放在源文件，因为那里才能看到完整的实现类型。
  std::unique_ptr<Impl> mImpl;
};

#endif
