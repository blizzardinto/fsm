/*
 * 阅读提示：对象之间传递的消息数据，包含发送者、接收者、类型、时间和附加指针。
 * 结构体适合把相关数据组合起来；附加指针没有类型检查和所有权，发送双方必须约定用途。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef TELEGRAM_H
#define TELEGRAM_H
// 消息结构体把对象通信所需的数据放在一起。原作者：Mat Buckland（fup@ai-junkie.com）。
#include <iostream>
#include <math.h>


struct Telegram
{
  // 发送者的实体编号。
  int          sender;

  // 接收者的实体编号。
  int          receiver;

  // 消息类型编号，足球消息定义在 SoccerMessages.h 中。
  int          msg;

  // 延迟消息的计划发送时刻。
  double       dispatchTime;

  // 借用的附加数据指针；不保存类型，也不拥有数据，发送双方必须约定类型与有效期。
  void*        extraInfo;


  Telegram():dispatchTime(-1),
                  sender(-1),
                  receiver(-1),
                  msg(-1)
  {}


  Telegram(double time,
           int    sender,
           int    receiver,
           int    msg,
           void*  info = NULL): dispatchTime(time),
                               sender(sender),
                               receiver(receiver),
                               msg(msg),
                               extraInfo(info)
  {}

};


// 为延迟消息的排序和去重提供比较规则；时间差小于阈值且发送者、接收者、类型相同的消息被视为相等。
const double smallestDelay = 0.25;


inline bool operator==(const Telegram& t1, const Telegram& t2)
{
  return ( fabs(t1.dispatchTime-t2.dispatchTime) < smallestDelay) &&
          (t1.sender == t2.sender)        &&
          (t1.receiver == t2.receiver)    &&
          (t1.msg == t2.msg);
}

inline bool operator<(const Telegram& t1, const Telegram& t2)
{
  if (t1 == t2)
  {
    return false;
  }

  else
  {
    return  (t1.dispatchTime < t2.dispatchTime);
  }
}

inline std::ostream& operator<<(std::ostream& os, const Telegram& t)
{
  os << "time: " << t.dispatchTime << "  Sender: " << t.sender
     << "   Receiver: " << t.receiver << "   Msg: " << t.msg;

  return os;
}

// 根据发送双方约定的类型转换附加指针，再取出对应数据；调用方必须保证类型正确且指针有效。
template <class T>
inline T dereferenceToType(void* p)
{
  return *(T*)(p);
}


#endif