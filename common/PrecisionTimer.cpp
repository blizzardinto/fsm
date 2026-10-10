#include "PrecisionTimer.h"


//---------------------- default function Object() { [native code] } ------------------------------
//
//-------------------------------------------------------------------------
PrecisionTimer::PrecisionTimer(): mNormalFps(0.0),
                  mSlowFps(1.0),
                  mTimeElapsed(0.0),
                  mFrameTime(0),
                  mLastTime(0),
                  mLastTimeInTimeElapsed(0),
                  mPerfCountFreq(0),
                  mStarted(false),
                  mStartTime(0),
                  mLastTimeElapsed(0.0),
                  mSmoothUpdates(false)
{
  //how many ticks per sec do we get
  QueryPerformanceFrequency( (LARGE_INTEGER*) &mPerfCountFreq);

  mTimeScale = 1.0/mPerfCountFreq;
}

//---------------------- function Object() { [native code] } -------------------------------------
//
//  use to specify FPS
//
//-------------------------------------------------------------------------
PrecisionTimer::PrecisionTimer(double fps): mNormalFps(fps),
                  mSlowFps(1.0),
                  mTimeElapsed(0.0),
                  mFrameTime(0),
                  mLastTime(0),
                  mLastTimeInTimeElapsed(0),
                  mPerfCountFreq(0),
                  mStarted(false),
                  mStartTime(0),
                  mLastTimeElapsed(0.0),
                  mSmoothUpdates(false)
{

  //how many ticks per sec do we get
  QueryPerformanceFrequency( (LARGE_INTEGER*) &mPerfCountFreq);

  mTimeScale = 1.0/mPerfCountFreq;

  //calculate ticks per frame
  mFrameTime = (LONGLONG)(mPerfCountFreq / mNormalFps);
}




//------------------------start()-----------------------------------------
//
//  call this immediately prior to game loop. Starts the timer (obviously!)
//
//--------------------------------------------------------------------------
void PrecisionTimer::start()
{
  mStarted = true;

  mTimeElapsed = 0.0;

  //get the time
  QueryPerformanceCounter( (LARGE_INTEGER*) &mLastTime);

  //keep a record of when the timer was started
  mStartTime = mLastTimeInTimeElapsed = mLastTime;

  //update time to render next frame
  mNextTime = mLastTime + mFrameTime;

  return;
}

