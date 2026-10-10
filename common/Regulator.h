/*
 * 阅读提示：动作频率调节器，封装下一次允许执行的时刻。
 * 每个使用者可以拥有自己的调节器，让踢球或支援评分按各自频率执行。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef REGULATOR
#define REGULATOR
#pragma comment(lib,"winmm.lib") // 非微软编译器构建时，也需要链接 Windows 多媒体计时库。
#include "mmsystem.h"

#include "utils.h"




class Regulator
{
private:

  // 两次允许更新之间的时间间隔。
  double mUpdatePeriod;

  // 下一次允许执行动作的时刻。
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


  // 当前时刻超过下一次更新时间时，允许执行。
  bool isReady()
  {
    // 更新频率为零时不限制执行频率。
    if (isEqual(0.0, mUpdatePeriod)) return true;

    // 更新频率为负数时，始终禁止执行。
    if (mUpdatePeriod < 0) return false;

    DWORD currentTime = timeGetTime();

    // 为更新间隔加入少量随机变化，避免多个对象总在同一时刻执行动作。
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