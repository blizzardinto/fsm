/*
 * 阅读提示：高精度计时器对象，封装计数频率、起点和下一帧时刻。
 * 主循环通过公开方法询问能否更新，无需直接操作计时器内部数据。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
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

  // 平滑模式处理异常短的时间间隔，减轻菜单操作、窗口移动等对计时的影响。
  bool      mSmoothUpdates;


public:

  // 构造函数负责初始化计时数据。
  PrecisionTimer();
  PrecisionTimer(double fps);


  // 启动计时器。
  void    start();

  // 判断是否已经经过一帧所需的时间。
  inline bool    readyForNextFrame();

  // 经过时间的读取应配合更新计时的方法使用。

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


// 固定帧率模式：到达下一帧时刻才返回真。
inline bool PrecisionTimer::readyForNextFrame()
{
  assert(mNormalFps && "PrecisionTimer::ReadyForNextFrame<No FPS set in timer>");

  QueryPerformanceCounter( (LARGE_INTEGER*) &mCurrentTime);

  if (mCurrentTime > mNextTime)
  {

    mTimeElapsed = (mCurrentTime - mLastTime) * mTimeScale;
    mLastTime    = mCurrentTime;

    // 计算下一帧允许执行的时刻。
    mNextTime = mCurrentTime + mFrameTime;

    return true;
  }

  return false;
}

// 返回距上次调用经过的秒数。
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


