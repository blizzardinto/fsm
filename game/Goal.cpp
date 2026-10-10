/*
 * 阅读提示：球门对象，把门柱、朝向和进球计数封装在一起。
 * 用足球前后两个位置检测穿越门线，避免高速运动时只检查当前位置而漏掉进球。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "Goal.h"

#include "SoccerBall.h"
#include "geometry.h"

Goal::Goal(Vector2D left, Vector2D right, Vector2D facing)
  : mLeftPost(left),
    mRightPost(right),
    mFacing(facing),
    mCenter((left + right) / 2.0),
    mNumGoalsScored(0)
{}

bool Goal::scored(const SoccerBall*const ball)
{
  if (lineIntersection2D(ball->pos(), ball->oldPos(), mLeftPost, mRightPost))
  {
    ++mNumGoalsScored;
    return true;
  }

  return false;
}

Vector2D Goal::center()const
{
  return mCenter;
}

Vector2D Goal::facing()const
{
  return mFacing;
}

Vector2D Goal::leftPost()const
{
  return mLeftPost;
}

Vector2D Goal::rightPost()const
{
  return mRightPost;
}

int Goal::numGoalsScored()const
{
  return mNumGoalsScored;
}

void Goal::resetGoalsScored()
{
  mNumGoalsScored = 0;
}
