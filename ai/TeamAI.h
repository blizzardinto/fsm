/*
 * 阅读提示：球队的决策控制器，负责进攻、防守和开球准备策略。
 * 它与球员控制器职责不同：球队决定整体安排，球员决定自己的具体动作。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef TEAMAI_H
#define TEAMAI_H

#include <memory>

class SoccerTeam;
struct Telegram;

// 枚举限定此角色能够使用的状态，编译器可以阻止把另一角色的枚举传进来。
enum class TeamState
{
  attacking,
  defending,
  prepareForKickOff
};

// 控制器把角色专属枚举转换成自己的状态对象；外部无需认识具体状态类。
class TeamAI
{
public:
  // 引用绑定已有业务对象，不复制它。explicit 防止对象被意外地隐式转换成控制器。
  explicit TeamAI(SoccerTeam& owner);
  ~TeamAI();
  TeamAI(const TeamAI&) = delete;
  TeamAI& operator=(const TeamAI&) = delete;

  void initialize(TeamState initialState);
  void update();
  bool handleMessage(const Telegram& message);
  void changeState(TeamState nextState);
  void revertToPreviousState();
  bool isInState(TeamState state) const;
  const char* getNameOfCurrentState() const;

private:
  // 只声明内部实现类型。具体状态成员放在实现文件，使用者无需包含全部状态头文件。
  struct Impl;
  // 独占智能指针负责销毁内部实现；析构放在源文件，因为那里才能看到完整的实现类型。
  std::unique_ptr<Impl> mImpl;
};

#endif
