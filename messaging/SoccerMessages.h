/*
 * 阅读提示：足球通信使用的消息编号与名称映射。
 * 具名枚举让传球、回到区域等请求表达清楚，避免用难以理解的数字直接表示动作。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef SOCCER_MESSAGES_H
#define SOCCER_MESSAGES_H

#include <string>

enum MessageType
{
  msgReceiveBall,
  msgPassToMe,
  msgSupportAttacker,
  msgGoHome,
  msgWait
};

// 将消息枚举转换为可读名称。
inline std::string messageToString(int msg);


#endif