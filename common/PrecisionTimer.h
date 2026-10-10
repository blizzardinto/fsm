#ifndef PRECISION_TIMER_H
#define PRECISION_TIMER_H
#include <windows.h>
#include <cassert>


class PrecisionTimer
{

private:

  LONGLONG  mCurrentTime,
            mLastTime,
            mLastTimeInTimeElapsed,
            mNextTime,
            mStartTime,
            mFrameTime,
            mPerfCountFreq;

  double    mTimeElapsed,
            mLastTimeElapsed,
            mTimeScale;

  double    mNormalFps;
  double    mSlowFps;

  bool      mStarted;

  //if true a call to timeElapsed() will return 0 if the current
  //time elapsed is much smaller than the previous. Used to counter
  //the problems associated with the user using menus/resizing/moving
  //a window etc
  bool      mSmoothUpdates;


public:

  //ctors
  PrecisionTimer();
  PrecisionTimer(double fps);


  //whatdayaknow, this starts the timer
  void    start();

  //determines if enough time has passed to move onto next frame
  inline bool    readyForNextFrame();

  //only use this after a call to the above.
  //double  GetTimeElapsed(){return mTimeElapsed;}

  inline double  timeElapsed();

  double  currentTime()
  {
    QueryPerformanceCounter( (LARGE_INTEGER*) &mCurrentTime);

    return (mCurrentTime - mStartTime) * mTimeScale;
  }

  bool    started()const{return mStarted;}

  void    smoothUpdatesOn(){mSmoothUpdates = true;}
  void    smoothUpdatesOff(){mSmoothUpdates = false;}

};


//-------------------------readyForNextFrame()-------------------------------
//
//  returns true if it is time to move on to the next frame step. to be used if
//  FPS is set.
//
//----------------------------------------------------------------------------
inline bool PrecisionTimer::readyForNextFrame()
{
  assert(mNormalFps && "PrecisionTimer::ReadyForNextFrame<No FPS set in timer>");

  QueryPerformanceCounter( (LARGE_INTEGER*) &mCurrentTime);

  if (mCurrentTime > mNextTime)
  {

    mTimeElapsed = (mCurrentTime - mLastTime) * mTimeScale;
    mLastTime    = mCurrentTime;

    //update time to render next frame
    mNextTime = mCurrentTime + mFrameTime;

    return true;
  }

  return false;
}

//--------------------------- timeElapsed --------------------------------
//
//  returns time elapsed since last call to this function.
//-------------------------------------------------------------------------
inline double PrecisionTimer::timeElapsed()
{
  mLastTimeElapsed = mTimeElapsed;

  QueryPerformanceCounter( (LARGE_INTEGER*) &mCurrentTime);

  mTimeElapsed = (mCurrentTime - mLastTimeInTimeElapsed) * mTimeScale;

  mLastTimeInTimeElapsed    = mCurrentTime;

  const double smoothness = 5.0;

  if (mSmoothUpdates)
  {
    if (mTimeElapsed < (mLastTimeElapsed * smoothness))
    {
      return mTimeElapsed;
    }

    else
    {
      return 0.0;
    }
  }

  else
  {
    return mTimeElapsed;
  }

}



#endif


