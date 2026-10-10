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
