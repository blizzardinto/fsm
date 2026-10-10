#ifndef FRAMECOUNTER_H
#define FRAMECOUNTER_H


#define tickCounter FrameCounter::instance()

class FrameCounter
{
private:

  long mCount;

  int  mFramesElapsed;

  FrameCounter():mCount(0), mFramesElapsed(0){}

  //copy ctor and assignment should be private
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