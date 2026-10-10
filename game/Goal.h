#ifndef GOAL_H
#define GOAL_H
#include "SoccerBall.h"
#include "Vector2D.h"
#include "geometry.h"



class Goal
{

private:

  Vector2D   mLeftPost;
  Vector2D   mRightPost;

  //a vector representing the facing direction of the goal
  Vector2D   mFacing;

  //the position of the center of the goal line
  Vector2D   mCenter;

  //each time scored() detects a goal this is incremented
  int        mNumGoalsScored;

public:

  Goal(Vector2D left, Vector2D right, Vector2D facing):mLeftPost(left),
                                                       mRightPost(right),
                                                       mCenter((left+right)/2.0),
                                                       mNumGoalsScored(0),
                                                       mFacing(facing)
  {  }

  //Given the current ball position and the previous ball position,
  //this method returns true if the ball has crossed the goal line
  //and increments mNumGoalsScored
  inline bool scored(const SoccerBall*const ball);

  //-----------------------------------------------------accessor methods
  Vector2D center()const{return mCenter;}
  Vector2D facing()const{return mFacing;}
  Vector2D leftPost()const{return mLeftPost;}
  Vector2D rightPost()const{return mRightPost;}

  int      numGoalsScored()const{return mNumGoalsScored;}
  void     resetGoalsScored(){mNumGoalsScored = 0;}
};


/////////////////////////////////////////////////////////////////////////

bool Goal::scored(const SoccerBall*const ball)
{
  if (lineIntersection2D(ball->pos(), ball->oldPos(), mLeftPost, mRightPost))
  {
    ++mNumGoalsScored;

    return true;
  }

  return false;
}


#endif