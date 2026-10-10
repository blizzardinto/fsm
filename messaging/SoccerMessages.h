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

//converts an enumerated value to a string
inline std::string messageToString(int msg);


#endif