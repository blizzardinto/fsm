/*
 * 阅读提示：帧计数器服务，记录模拟推进次数，为按帧计算的逻辑提供共同刻度。
 * 静态单例由调用者共享，不属于某个球员。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef FRAMECOUNTER_H
#define FRAMECOUNTER_H


#define tickCounter FrameCounter::instance()

class FrameCounter
{
private:

  long mCount;

  int  mFramesElapsed;

  FrameCounter():mCount(0), mFramesElapsed(0){}

  // 禁止复制和赋值，统一使用帧计数器的单例。
  FrameCounter(const FrameCounter&);
  FrameCounter& operator=(const FrameCounter&);

public:

  static FrameCounter* instance();

  void update(){++mCount; ++mFramesElapsed;}

  long getCurrentFrame(){return mCount;}

  void reset(){mCount = 0;}

  void start(){mFramesElapsed = 0;}
  int  framesElapsedSinceStartCalled()const{return mFramesElapsed;}

};

#endif