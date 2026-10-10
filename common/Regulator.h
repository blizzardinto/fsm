#ifndef REGULATOR
#define REGULATOR
#pragma comment(lib,"winmm.lib") //if you don't use MSVC make sure this library is included in your project
#include "mmsystem.h"

#include "utils.h"




class Regulator
{
private:

  //the time period between updates
  double mUpdatePeriod;

  //the next time the regulator allows code flow
  DWORD mNextUpdateTime;


public:


  Regulator(double numUpdatesPerSecondRqd)
  {
    mNextUpdateTime = (DWORD)(timeGetTime()+randFloat()*1000);

    if (numUpdatesPerSecondRqd > 0)
    {
      mUpdatePeriod = 1000.0 / numUpdatesPerSecondRqd;
    }

    else if (isEqual(0.0, numUpdatesPerSecondRqd))
    {
      mUpdatePeriod = 0.0;
    }

    else if (numUpdatesPerSecondRqd < 0)
    {
      mUpdatePeriod = -1;
    }
  }


  //returns true if the current time exceeds mNextUpdateTime
  bool isReady()
  {
    //if a regulator is instantiated with a zero freq then it goes into
    //stealth mode (doesn't regulate)
    if (isEqual(0.0, mUpdatePeriod)) return true;

    //if the regulator is instantiated with a negative freq then it will
    //never allow the code to flow
    if (mUpdatePeriod < 0) return false;

    DWORD currentTime = timeGetTime();

    //the number of milliseconds the update period can vary per required
    //update-step. This is here to make sure any multiple clients of this class
    //have their updates spread evenly
    static const double updatePeriodVariator = 10.0;

    if (currentTime >= mNextUpdateTime)
    {
      mNextUpdateTime = (DWORD)(currentTime + mUpdatePeriod + randInRange(-updatePeriodVariator, updatePeriodVariator));

      return true;
    }

    return false;
  }
};



#endif