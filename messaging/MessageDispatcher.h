#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  name:   MessageDispatcher.h
//
//  Desc:   A message dispatcher. Manages messages of the type Telegram.
//          Instantiated as a singleton.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <set>
#include <string>


#include "Telegram.h"


class EntityBase;


//to make life easier...
#define dispatcher MessageDispatcher::instance()

//to make code easier to read
const double sendMsgImmediately = 0.0;
const int    noAdditionalInfo   = 0;
const int    senderIdIrrelevant = -1;


class MessageDispatcher
{
private:

  //a std::set is used as the container for the delayed messages
  //because of the benefit of automatic sorting and avoidance
  //of duplicates. Messages are sorted by their dispatch time.
  std::set<Telegram> mDelayedMessages;

  //this method is utilized by dispatchMsg or dispatchDelayedMessages.
  //This method calls the message handling member function of the receiving
  //entity, pReceiver, with the newly created telegram
  void discharge(EntityBase* pReceiver, const Telegram& msg);

  MessageDispatcher(){}

  //copy ctor and assignment should be private
  MessageDispatcher(const MessageDispatcher&);
  MessageDispatcher& operator=(const MessageDispatcher&);

public:

  static MessageDispatcher* instance();

  //send a message to another agent. Receiving agent is referenced by id.
  void dispatchMsg(double      delay,
                   int         sender,
                   int         receiver,
                   int         msg,
                   void*       extraInfo);

  //send out any delayed messages. This method is called each time through
  //the main game loop.
  void dispatchDelayedMessages();
};



#endif