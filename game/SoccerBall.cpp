/*
 * 阅读提示：足球对象，复用可移动实体的数据，封装踢球、摩擦、反弹和位置预测。
 * 球员请求踢球时只调用公开接口，不应直接修改足球内部速度，这体现了封装。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "SoccerBall.h"
#include "geometry.h"
#include "DebugConsole.h"
#include "Cgdi.h"
#include "ParamLoader.h"
#include "Wall2D.h"


// 踢球前按精度参数扰动目标方向，模拟踢球误差。
Vector2D addNoiseToKick(Vector2D ballPos, Vector2D ballTarget)
{

  double displacement = (pi - pi*prm.playerKickingAccuracy) * randomClamped();

  Vector2D toTarget = ballTarget - ballPos;

  vec2DRotateAroundOrigin(toTarget, displacement);

  return toTarget + ballPos;
}



// 踢球：把踢球力量除以质量得到速度大小，并沿给定方向设置足球速度。
void SoccerBall::kick(Vector2D direction, double force)
{
  // 把方向变成单位向量，防止向量长度影响踢球力量。
  direction.normalize();

  // 力量除以质量得到本次踢球的速度增量尺度。
  Vector2D acceleration = (direction * force) / mMass;

  // 沿踢球方向设置速度。
  mVelocity = acceleration;
}

// 更新足球物理：保存旧位置、检测墙壁碰撞、施加摩擦并更新位置。
void SoccerBall::update()
{
  // 保存上一位置，让球门可以检测这一轮移动是否穿越门线。
  mOldPos = mPosition;

      // 检测与场地边界的碰撞。
    testCollisionWithWalls(mPitchBoundary);

  // 足球仍在移动时，沿速度方向施加摩擦减速。
  if (mVelocity.lengthSq() > prm.friction * prm.friction)
  {
    mVelocity += vec2DNormalize(mVelocity) * prm.friction;

    mPosition += mVelocity;



    // 根据非零速度更新朝向。
    mHeading = vec2DNormalize(mVelocity);
  }
}

// 按踢球力量和摩擦估算足球从起点到终点所需的时间；无法到达时返回负值。
double SoccerBall::timeToCoverDistance(Vector2D pointA,
                                      Vector2D pointB,
                                      double force)const
{
  // 踢球力量除以质量，得到传球后的初速度。
  double speed = force / mMass;

  // 先求距离，再使用匀加速公式：末速度平方等于初速度平方加二倍加速度乘距离。
  double distanceToCover =  vec2DDistance(pointA, pointB);

  double term = speed*speed + 2.0*distanceToCover*prm.friction;

  // 末速度平方不大于零时，本实现将该传球视为不可行并返回负值。
  if (term <= 0.0) return -1.0;

  double v = sqrt(term);

  // 已知初速度、末速度和摩擦加速度，用速度差除以加速度计算时间。
  return (v-speed)/prm.friction;
}

// 根据当前速度和摩擦预测一段时间后的足球位置。
Vector2D SoccerBall::futurePosition(double time)const
{
  // 位移公式为初速度乘时间加一半加速度乘时间平方；先计算速度向量对应的部分。
  Vector2D ut = mVelocity * time;

  // 计算摩擦造成的标量位移。
  double halfAccelerationTimeSquared = 0.5 * prm.friction * time * time;

  // 将摩擦位移乘以速度单位向量，得到沿运动方向的向量。
  Vector2D scalarToVector = halfAccelerationTimeSquared * vec2DNormalize(mVelocity);

  // 当前位置加上两个位移项，得到预测位置。
  return pos() + ut + scalarToVector;
}


// 绘制足球。
void SoccerBall::render()
{
  gdi->blackBrush();

  gdi->circle(mPosition, mBoundingRadius);

  /* 调试时可额外绘制碰撞交点；正式绘制只显示足球。 */
}


// 检查足球下一步运动是否撞到场地墙壁。
void SoccerBall::testCollisionWithWalls(const std::vector<Wall2D>& walls)
{
  // 遍历墙壁，寻找最近的有效碰撞。
  int idxClosest = -1;

  Vector2D velNormal = vec2DNormalize(mVelocity);

  Vector2D intersectionPoint, collisionPoint;

  double distToIntersection = maxFloat;

  // 检查各墙壁是否相交，并记录最近碰撞墙壁的编号。
  for (unsigned int w=0; w<walls.size(); ++w)
  {
    // 从球心沿墙壁法线的反方向移动一个半径，得到球面上的候选碰撞点。
    Vector2D thisCollisionPoint = pos() - (walls[w].normal() * boundingRadius());

    // 计算候选点沿运动方向与墙壁所在直线的交点。
    if (whereIsPoint(thisCollisionPoint,
                     walls[w].from(),
                     walls[w].normal()) == planeBackside)
    {
      double distToWall = distanceToRayPlaneIntersection(thisCollisionPoint,
                                                         walls[w].normal(),
                                                         walls[w].from(),
                                                         walls[w].normal());

      intersectionPoint = thisCollisionPoint + (distToWall * walls[w].normal());

    }

    else
    {
      double distToWall = distanceToRayPlaneIntersection(thisCollisionPoint,
                                                         velNormal,
                                                         walls[w].from(),
                                                         walls[w].normal());

      intersectionPoint = thisCollisionPoint + (distToWall * velNormal);
    }

    // 检查交点是否落在实际墙壁线段上。
    bool onLineSegment = false;

    if (lineIntersection2D(walls[w].from(),
                           walls[w].to(),
                           thisCollisionPoint - walls[w].normal()*20.0,
                           thisCollisionPoint + walls[w].normal()*20.0))
    {

      onLineSegment = true;
    }


                                                                          // 此算法不检测线段端点碰撞；用距离平方检查碰撞能否在本次移动内发生，并保留最近的一次。
    double distSq = vec2DDistanceSq(thisCollisionPoint, intersectionPoint);

    if ((distSq <= mVelocity.lengthSq()) && (distSq < distToIntersection) && onLineSegment)
    {
      distToIntersection = distSq;
      idxClosest = w;
      collisionPoint = intersectionPoint;
    }
  }// 仅当速度朝向墙壁时才反射，避免足球越过边界后被连续反射。
  if ( (idxClosest >= 0 ) && velNormal.dot(walls[idxClosest].normal()) < 0)
  {
    mVelocity.reflect(walls[idxClosest].normal());
  }
}

// 把足球放到指定位置，并将速度清零。
void SoccerBall::placeAtPosition(Vector2D newPos)
{
  mPosition = newPos;

  mOldPos = mPosition;

  mVelocity.zero();
}

