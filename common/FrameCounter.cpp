/*
 * 阅读提示：帧计数器服务，记录模拟推进次数，为按帧计算的逻辑提供共同刻度。
 * 静态单例由调用者共享，不属于某个球员。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "framecounter.h"


FrameCounter* FrameCounter::instance()
{
  static FrameCounter instance;

  return &instance;
}