/*
 * 阅读提示：状态机单元测试，用探针状态代替足球状态并记录回调顺序。
 * 测试依赖抽象接口而非真实比赛，展示了解耦如何让调度器可以单独验证。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "State.h"
#include "StateMachine.h"
#include "Telegram.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
// 不依赖断言宏，条件失败时抛出异常，发布构建中也能发现测试失败。
void require(bool condition, const char* message)
{
  if (!condition) throw std::runtime_error(message);
}

// 测试替身重写虚函数，只记录调用，不执行足球逻辑；用于观察多态调度的顺序。
class ProbeState : public State
{
public:
  ProbeState(const char* label, std::vector<std::string>& trace)
    : mLabel(label), mTrace(trace)
  {}

  void enter() override
  {
    record("enter");
    if (onEnter) onEnter();
  }
  void execute() override
  {
    record("execute");
    if (onExecute) onExecute();
  }
  void exit() override { record("exit"); }
  bool onMessage(const Telegram&) override
  {
    record("message");
    return handlesMessages;
  }
  const char* name() const override { return mLabel; }

  bool handlesMessages = false;
  // 可注入函数让测试在进入或执行期间触发切换，验证嵌套调用行为。
  std::function<void()> onEnter;
  std::function<void()> onExecute;

private:
  void record(const char* action) { mTrace.push_back(std::string(mLabel) + "." + action); }
  const char* mLabel;
  std::vector<std::string>& mTrace;
};

void expect(const std::vector<std::string>& trace,
            const std::vector<std::string>& expected)
{
  require(trace == expected, "Unexpected callback order");
}
}

int main()
{
  try
  {
    std::vector<std::string> trace;
    ProbeState first("first", trace), second("second", trace), global("global", trace);
    StateMachine machine;
    Telegram message(0, 1, 2, 3);

    machine.update();
    machine.revertToPreviousState();
    require(!machine.handleMessage(message), "Empty machine handled a message");
    require(std::string(machine.getNameOfCurrentState()) == "None", "Empty machine name");

    machine.initialize(first, &global);
    expect(trace, {"first.enter"});
    require(machine.previousState() == &first, "Initial previous state");
    trace.clear();
    machine.update();
    expect(trace, {"global.execute", "first.execute"});

    trace.clear();
    machine.changeState(second);
    expect(trace, {"first.exit", "second.enter"});
    require(machine.isInState(second) && !machine.isInState(first), "State identity");
    trace.clear();
    machine.revertToPreviousState();
    expect(trace, {"second.exit", "first.enter"});

    trace.clear();
    first.handlesMessages = true;
    require(machine.handleMessage(message), "Current handler was ignored");
    expect(trace, {"first.message"});
    first.handlesMessages = false;
    global.handlesMessages = true;
    trace.clear();
    require(machine.handleMessage(message), "Global fallback was ignored");
    expect(trace, {"first.message", "global.message"});
    global.handlesMessages = false;
    require(!machine.handleMessage(message), "Unhandled message result");

    trace.clear();
    global.onExecute = [&] { machine.changeState(second); };
    machine.update();
    expect(trace, {"global.execute", "first.exit", "second.enter", "second.execute"});
    global.onExecute = {};

    trace.clear();
    machine.changeState(second);
    expect(trace, {"second.exit", "second.enter"});

    trace.clear();
    first.onEnter = [&] { machine.changeState(second); };
    machine.changeState(first);
    expect(trace, {"second.exit", "first.enter", "first.exit", "second.enter"});
    require(machine.previousState() == &first && machine.currentState() == &second,
            "Nested entry transition");
    first.onEnter = {};

    std::vector<std::string> otherTrace;
    ProbeState other("first", otherTrace);
    StateMachine otherMachine;
    otherMachine.initialize(other, nullptr, false);
    require(otherTrace.empty(), "Deferred entry ran a callback");
    require(!otherMachine.isInState(first), "Same class/name shared state identity");
    require(otherMachine.currentState() == &other && machine.currentState() == &second,
            "Independent machines interfered");
    std::cout << "StateMachine tests passed\n";
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
