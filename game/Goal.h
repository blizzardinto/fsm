#ifndef GOAL_H
#define GOAL_H
#include "Vector2D.h"

class SoccerBall;
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

  Goal(Vector2D left, Vector2D right, Vector2D facing);

  //Given the current ball position and the previous ball position,
  //this method returns true if the ball has crossed the goal line
  //and increments mNumGoalsScored
  bool scored(const SoccerBall*const ball);

  //-----------------------------------------------------accessor methods
  Vector2D center()const;
  Vector2D facing()const;
  Vector2D leftPost()const;
  Vector2D rightPost()const;

  int      numGoalsScored()const;
  void     resetGoalsScored();
};
#endif
