/*
 * 阅读提示：高精度计时器对象，封装计数频率、起点和下一帧时刻。
 * 主循环通过公开方法询问能否更新，无需直接操作计时器内部数据。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "PrecisionTimer.h"


// 默认构造函数：初始化计时器内部数据。
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
  // 查询高精度计数器每秒计数次数。
  QueryPerformanceFrequency( (LARGE_INTEGER*) &mPerfCountFreq);

  mTimeScale = 1.0/mPerfCountFreq;
}

// 指定帧率的构造函数：计算每帧需要的计数间隔。
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

  // 查询高精度计数器每秒计数次数。
  QueryPerformanceFrequency( (LARGE_INTEGER*) &mPerfCountFreq);

  mTimeScale = 1.0/mPerfCountFreq;

  // 根据帧率计算每帧所需的计数次数。
  mFrameTime = (LONGLONG)(mPerfCountFreq / mNormalFps);
}




// 主循环开始前调用 start，记录计时起点。
void PrecisionTimer::start()
{
  mStarted = true;

  mTimeElapsed = 0.0;

  // 读取当前计数值。
  QueryPerformanceCounter( (LARGE_INTEGER*) &mLastTime);

  // 保存计时器开始的时刻。
  mStartTime = mLastTimeInTimeElapsed = mLastTime;

  // 设置下一帧允许执行的时刻。
  mNextTime = mLastTime + mFrameTime;

  return;
}

