#include "MessageDispatcher.h"
#include "EntityBase.h"
#include "FrameCounter.h"
#include "EntityManager.h"
#include "DebugConsole.h"

using std::set;

//uncomment below to send message info to the debug window
//#define SHOW_MESSAGING_INFO

//--------------------------- instance ----------------------------------------
//
//   this class is a singleton
//-----------------------------------------------------------------------------
MessageDispatcher* MessageDispatcher::instance()
{
  static MessageDispatcher instance;

  return &instance;
}

//----------------------------- Dispatch ---------------------------------
//
//  see description in header
//------------------------------------------------------------------------
void MessageDispatcher::discharge(EntityBase* pReceiver, const Telegram& telegram)
{
  if (!pReceiver->handleMessage(telegram))
  {
    //telegram could not be handled
    #ifdef SHOW_MESSAGING_INFO
    debugCon << "Message not handled" << "";
    #endif
  }
}

//---------------------------- dispatchMsg ---------------------------
//
//  given a message, a receiver, a sender and any time delay, this function
//  routes the message to the correct agent (if no delay) or stores
//  in the message queue to be dispatched at the correct time
//------------------------------------------------------------------------
void MessageDispatcher::dispatchMsg(double       delay,
                                    int          sender,
                                    int          receiver,
                                    int          msg,
                                    void*        additionalInfo = NULL)
{

  //get a pointer to the receiver
  EntityBase* pReceiver = entityMgr->getEntityFromId(receiver);

  //make sure the receiver is valid
  if (pReceiver == NULL)
  {
    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nWarning! No Receiver with ID of " << receiver << " found" << "";
    #endif

    return;
  }

  //create the telegram
  Telegram telegram(0, sender, receiver, msg, additionalInfo);

  //if there is no delay, route telegram immediately
  if (delay <= 0.0)
  {
    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nTelegram dispatched at time: " << tickCounter->getCurrentFrame()
         << " by " << sender << " for " << receiver
         << ". Msg is " << msg << "";
    #endif

    //send the telegram to the recipient
    discharge(pReceiver, telegram);
  }

  //else calculate the time when the telegram should be dispatched
  else
  {
    double currentTime = tickCounter->getCurrentFrame();

    telegram.dispatchTime = currentTime + delay;

    //and put it in the queue
    mDelayedMessages.insert(telegram);

    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nDelayed telegram from " << sender << " recorded at time "
            << tickCounter->getCurrentFrame() << " for " << receiver
            << ". Msg is " << msg << "";
    #endif
  }
}

//---------------------- dispatchDelayedMessages -------------------------
//
//  This function dispatches any telegrams with a timestamp that has
//  expired. Any dispatched telegrams are removed from the queue
//------------------------------------------------------------------------
void MessageDispatcher::dispatchDelayedMessages()
{
  //first get current time
  double currentTime = tickCounter->getCurrentFrame();

  //now peek at the queue to see if any telegrams need dispatching.
  //remove all telegrams from the front of the queue that have gone
  //past their sell by date
  while( !mDelayedMessages.empty() &&
       (mDelayedMessages.begin()->dispatchTime < currentTime) &&
         (mDelayedMessages.begin()->dispatchTime > 0) )
  {
    //read the telegram from the front of the queue
    const Telegram& telegram = *mDelayedMessages.begin();

    //find the recipient
    EntityBase* pReceiver = entityMgr->getEntityFromId(telegram.receiver);

    #ifdef SHOW_MESSAGING_INFO
    debugCon << "\nQueued telegram ready for dispatch: Sent to "
         << pReceiver->id() << ". Msg is "<< telegram.msg << "";
    #endif

    //send the telegram to the recipient
    discharge(pReceiver, telegram);

  //remove it from the queue
    mDelayedMessages.erase(mDelayedMessages.begin());
  }
}



