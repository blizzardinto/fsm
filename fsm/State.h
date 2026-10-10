/*
 * 阅读提示：抽象状态接口，规定所有状态必须提供哪些操作。
 * 纯虚函数 execute 的等号零表示基类不给实现，由派生状态决定行为；虚析构保证基类指针可以安全销毁派生对象。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef STATE_H
#define STATE_H

struct Telegram;

// 抽象状态接口：具体状态绑定业务对象，状态机通过虚函数调用不同状态的实现。
class State
{
public:
  virtual ~State();
  // 进入、执行、退出分别对应动作准备、持续决策和动作清理。默认进入和退出为空。
  virtual void enter();
  virtual void execute() = 0;
  virtual void exit();
  // 返回真表示已经处理消息；返回假允许状态机继续询问全局状态。
  virtual bool onMessage(const Telegram& message);
  virtual const char* name() const = 0;

protected:
  State() = default;
  // 禁止复制状态，避免把绑定业务对象的状态误当成普通数值复制。
  State(const State&) = delete;
  State& operator=(const State&) = delete;
};

#endif
