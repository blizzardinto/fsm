/*
 * 阅读提示：对象通信服务，封装按编号投递与延迟消息存储。
 * 发送者请求对方做事而不是直接操作对方状态；接收者通过自己的消息接口决定如何响应。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H
#pragma warning (disable:4786)
// 消息分发器封装即时发送和延迟队列。原作者：Mat Buckland（fup@ai-junkie.com）。
#include <set>
#include <string>


#include "Telegram.h"


class EntityBase;


// 提供消息分发器访问简写。
#define dispatcher MessageDispatcher::instance()

// 用具名常量提高调用代码的可读性。
const double sendMsgImmediately = 0.0;
const int    noAdditionalInfo   = 0;
const int    senderIdIrrelevant = -1;


class MessageDispatcher
{
private:

  // 使用有序集合保存延迟消息，按时间排序；相等判断还用于合并近时间的重复消息。
  std::set<Telegram> mDelayedMessages;

  // 投递的最终入口：调用接收实体的虚函数，由对象实际类型决定处理方法。
  void discharge(EntityBase* pReceiver, const Telegram& msg);

  MessageDispatcher(){}

  // 禁止复制和赋值，统一使用单例服务。
  MessageDispatcher(const MessageDispatcher&);
  MessageDispatcher& operator=(const MessageDispatcher&);

public:

  static MessageDispatcher* instance();

  // 按实体编号向另一个对象发送消息。
  void dispatchMsg(double      delay,
                   int         sender,
                   int         receiver,
                   int         msg,
                   void*       extraInfo);

  // 发送已到期的延迟消息；调用方需要定期调用此方法，排队不会自动触发发送。
  void dispatchDelayedMessages();
};



#endif