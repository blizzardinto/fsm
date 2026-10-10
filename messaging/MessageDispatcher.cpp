/*
 * 阅读提示：对象通信服务，封装按编号投递与延迟消息存储。
 * 发送者请求对方做事而不是直接操作对方状态；接收者通过自己的消息接口决定如何响应。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "MessageDispatcher.h"
#include "EntityBase.h"
#include "FrameCounter.h"
#include "EntityManager.h"
#include "DebugConsole.h"

using std::set;

// 消息分发器采用单例；启用 SHOW_MESSAGING_INFO 宏可观察消息传递。
MessageDispatcher* MessageDispatcher::instance()
{
  static MessageDispatcher instance;

  return &instance;
}

// 将消息交给接收实体的虚函数处理。
void MessageDispatcher::discharge(EntityBase* pReceiver, const Telegram& telegram)
{
  if (!pReceiver->handleMessage(telegram))
  {
    // 接收者返回假表示没有处理该消息。
    #ifdef SHOW_MESSAGING_INFO
    debugCon << "Message not handled" << "";
    #endif
  }
}

// 按编号查找接收者；即时消息立即发送，延迟消息记录时间后排队。
void MessageDispatcher::dispatchMsg(double       delay,
                                    int          sender,
                                    int          receiver,
                                    int          msg,
                                    void*        additionalInfo = NULL)
{

  // 根据实体编号取得接收者的借用指针。
  EntityBase* pReceiver = entityMgr->getEntityFromId(receiver);

  // 找不到接收者时放弃发送。
  if (pReceiver == NULL)
  {
    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nWarning! No Receiver with ID of " << receiver << " found" << "";
    #endif

    return;
  }

  // 将发送者、接收者、消息类型及附加数据组成消息对象。
  Telegram telegram(0, sender, receiver, msg, additionalInfo);

  // 延迟为零时同步调用接收者。
  if (delay <= 0.0)
  {
    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nTelegram dispatched at time: " << tickCounter->getCurrentFrame()
         << " by " << sender << " for " << receiver
         << ". Msg is " << msg << "";
    #endif

    // 通过接收者的消息接口投递。
    discharge(pReceiver, telegram);
  }

  // 延迟非零时计算将来的发送时刻。
  else
  {
    double currentTime = tickCounter->getCurrentFrame();

    telegram.dispatchTime = currentTime + delay;

    // 将消息放入按时间排序的容器。
    mDelayedMessages.insert(telegram);

    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nDelayed telegram from " << sender << " recorded at time "
            << tickCounter->getCurrentFrame() << " for " << receiver
            << ". Msg is " << msg << "";
    #endif
  }
}

// 发送已经到期的延迟消息，并移除发送完成的记录。
// 队列只保存消息数据，过期不会自行执行；必须由程序主动调用这个接口推进投递。
void MessageDispatcher::dispatchDelayedMessages()
{
  // 获取当前时间。
  double currentTime = tickCounter->getCurrentFrame();

  // 从队首开始处理所有已到期消息。
  while( !mDelayedMessages.empty() &&
       (mDelayedMessages.begin()->dispatchTime < currentTime) &&
         (mDelayedMessages.begin()->dispatchTime > 0) )
  {
    // 取出最早的消息。
    const Telegram& telegram = *mDelayedMessages.begin();

    // 根据编号查找接收者。
    EntityBase* pReceiver = entityMgr->getEntityFromId(telegram.receiver);

    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nQueued telegram ready for dispatch: Sent to "
         << pReceiver->id() << ". Msg is "<< telegram.msg << "";
    #endif

    // 调用接收者的消息处理接口。
    discharge(pReceiver, telegram);

  // 从队列中删除已发送的消息记录。
    mDelayedMessages.erase(mDelayedMessages.begin());
  }
}



