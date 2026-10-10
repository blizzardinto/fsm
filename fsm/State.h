#ifndef STATE_H
#define STATE_H
//------------------------------------------------------------------------
//
//  name:   State.h
//
//  Desc:   abstract base class to define an interface for a state
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
struct Telegram;

template <class entityType>
class State
{
public:

  virtual ~State(){}

  //this will execute when the state is entered
  virtual void enter(entityType*)=0;

  //this is the states normal update function
  virtual void execute(entityType*)=0;

  //this will execute when the state is exited.
  virtual void exit(entityType*)=0;

  //this executes if the agent receives a message from the
  //message dispatcher
  virtual bool onMessage(entityType*, const Telegram&)=0;
};

#endif