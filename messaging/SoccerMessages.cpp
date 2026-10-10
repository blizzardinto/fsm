/*
 * 阅读提示：足球通信使用的消息编号与名称映射。
 * 具名枚举让传球、回到区域等请求表达清楚，避免用难以理解的数字直接表示动作。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "SoccerMessages.h"


inline std::string messageToString(int msg)
{
  switch (msg)
  {
  case msgReceiveBall:

    return "Msg_ReceiveBall";

  case msgPassToMe:

    return "Msg_PassToMe";

  case msgSupportAttacker:

    return "Msg_SupportAttacker";

  case msgGoHome:

    return "Msg_GoHome";

  case msgWait:

    return "Msg_Wait";

  default:

    return "INVALID MESSAGE!!";
  }
}