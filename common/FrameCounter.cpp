#include "framecounter.h"


FrameCounter* FrameCounter::instance()
{
  static FrameCounter instance;

  return &instance;
}