/*
 * 阅读提示：球门对象，把门柱、朝向和进球计数封装在一起。
 * 用足球前后两个位置检测穿越门线，避免高速运动时只检查当前位置而漏掉进球。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef GOAL_H
#define GOAL_H
#include "Vector2D.h"

class SoccerBall;
class Goal
{

private:

  Vector2D   mLeftPost;
  Vector2D   mRightPost;

  // 球门面向场内的方向向量。
  Vector2D   mFacing;

  // 球门线中心位置。
  Vector2D   mCenter;

  // 每检测到一次有效进球，就增加此计数。
  int        mNumGoalsScored;

public:

  Goal(Vector2D left, Vector2D right, Vector2D facing);

  // 检查足球从上一位置到当前位置是否穿过球门线，并在进球时增加计数。
  bool scored(const SoccerBall*const ball);

  // 访问器：通过方法读取球门数据，封装内部成员。
  Vector2D center()const;
  Vector2D facing()const;
  Vector2D leftPost()const;
  Vector2D rightPost()const;

  int      numGoalsScored()const;
  void     resetGoalsScored();
};
#endif
