/*
 * 阅读提示：抽象状态接口，规定所有状态必须提供哪些操作。
 * 纯虚函数 execute 的等号零表示基类不给实现，由派生状态决定行为；虚析构保证基类指针可以安全销毁派生对象。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "State.h"

State::~State() = default;
void State::enter() {}
void State::exit() {}
bool State::onMessage(const Telegram&) { return false; }
