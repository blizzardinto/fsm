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