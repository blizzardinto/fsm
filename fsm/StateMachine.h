 #ifndef STATEMACHINE_H
#define STATEMACHINE_H

//------------------------------------------------------------------------
//
//  name:   StateMachine.h
//
//  Desc:   State machine class. Inherit from this class and create some
//          states to give your agents FSM functionality
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <cassert>
#include <string>

#include "State.h"
#include "Telegram.h"


template <class entityType>
class StateMachine
{
private:

  //a pointer to the agent that owns this instance
  entityType*          mOwner;

  State<entityType>*   mCurrentState;

  //a record of the last state the agent was in
  State<entityType>*   mPreviousState;

  //this is called every time the FSM is updated
  State<entityType>*   mGlobalState;


public:

  StateMachine(entityType* owner):mOwner(owner),
                                   mCurrentState(NULL),
                                   mPreviousState(NULL),
                                   mGlobalState(NULL)
  {}

  virtual ~StateMachine(){}

  //use these methods to initialize the FSM
  void setCurrentState(State<entityType>* s){mCurrentState = s;}
  void setGlobalState(State<entityType>* s) {mGlobalState = s;}
  void setPreviousState(State<entityType>* s){mPreviousState = s;}

  //call this to update the FSM
  void  update()const
  {
    //if a global state exists, call its execute method, else do nothing
    if(mGlobalState)   mGlobalState->execute(mOwner);

    //same for the current state
    if (mCurrentState) mCurrentState->execute(mOwner);
  }

  bool  handleMessage(const Telegram& msg)const
  {
    //first see if the current state is valid and that it can handle
    //the message
    if (mCurrentState && mCurrentState->onMessage(mOwner, msg))
    {
      return true;
    }

    //if not, and if a global state has been implemented, send
    //the message to the global state
    if (mGlobalState && mGlobalState->onMessage(mOwner, msg))
    {
      return true;
    }

    return false;
  }

  //change to a new state
  void  changeState(State<entityType>* pNewState)
  {
    assert(pNewState && "<StateMachine::ChangeState>:trying to assign null state to current");

    //keep a record of the previous state
    mPreviousState = mCurrentState;

    //call the exit method of the existing state
    mCurrentState->exit(mOwner);

    //change state to the new state
    mCurrentState = pNewState;

    //call the entry method of the new state
    mCurrentState->enter(mOwner);
  }

  //change state back to the previous state
  void  revertToPreviousState()
  {
    changeState(mPreviousState);
  }

  //returns true if the current state's type is equal to the type of the
  //class passed as a parameter.
  bool  isInState(const State<entityType>& st)const
  {
    if (typeid(*mCurrentState) == typeid(st)) return true;
    return false;
  }

  State<entityType>*  currentState()  const{return mCurrentState;}
  State<entityType>*  globalState()   const{return mGlobalState;}
  State<entityType>*  previousState() const{return mPreviousState;}

  //only ever used during debugging to grab the name of the current state
  std::string         getNameOfCurrentState()const
  {
    std::string s(typeid(*mCurrentState).name());

    //remove the 'class ' part from the front of the string
    if (s.size() > 5)
    {
      s.erase(0, 6);
    }

    return s;
  }
};




#endif


