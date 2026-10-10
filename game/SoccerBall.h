/*
 * 阅读提示：足球对象，复用可移动实体的数据，封装踢球、摩擦、反弹和位置预测。
 * 球员请求踢球时只调用公开接口，不应直接修改足球内部速度，这体现了封装。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef SOCCERBALL_H
#define SOCCERBALL_H
#pragma warning (disable:4786)
#include <vector>

#include "EntityMovable.h"
#include "constants.h"


class Wall2D;
class EntityPlayer;


class SoccerBall : public EntityMovable
{
private:

  // 保存上一轮更新时的位置，供进球检测使用。
  Vector2D                  mOldPos;

  // 借用球场边界墙壁容器；球场必须比足球活得更久。
  const std::vector<Wall2D>& mPitchBoundary;




public:
    // 检测墙壁碰撞，并据墙壁法线反射足球速度。
  void testCollisionWithWalls(const std::vector<Wall2D>& walls);

  SoccerBall(Vector2D           pos,
             double               ballSize,
             double               mass,
             std::vector<Wall2D>& pitchBoundary):

      // 通过初始化列表设置可移动实体基类的数据。
      EntityMovable(pos,
                  ballSize,
                  Vector2D(0,0),
                  -1.0,                // 最大速度参数在足球实现中不使用。
                  Vector2D(0,1),
                  mass,
                  Vector2D(1.0,1.0),  // 缩放参数在足球实现中不使用。
                  0,                   // 转向参数在足球实现中不使用。
                  0),                  // 最大驱动力参数在足球实现中不使用。
     mPitchBoundary(pitchBoundary)
  {}

  // 重写基类更新接口，提供足球的物理运动。
  void      update();

  // 重写基类绘制接口。
  void      render();

  // 足球目前忽略消息并返回未处理。
  bool      handleMessage(const Telegram& msg){return false;}

  // 按指定方向与力量踢球。
  void      kick(Vector2D direction, double force);

  // 估算指定力量下足球从起点到终点所需时间，无法到达时返回负值。
  double    timeToCoverDistance(Vector2D from,
                               Vector2D to,
                               double     force)const;

  // 预测指定时间后的足球位置。
  Vector2D futurePosition(double time)const;

  // 接球时直接将足球速度清零；控球者关系由球队和 AI 状态另外维护。
  void      trap(){mVelocity.zero();}

  Vector2D  oldPos()const{return mOldPos;}

  // 放置足球并清除速度。
  void      placeAtPosition(Vector2D newPos);
};



// 根据球员精度调整踢球目标。
Vector2D addNoiseToKick(Vector2D ballPos, Vector2D ballTarget);



#endif