/*
 * 阅读提示：守门员具体类，继承球员公共能力，并拥有 GoalkeeperAI 控制器。
 * 球队通过 EntityPlayer 指针统一更新成员，虚函数会自动选择守门员的实现。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef GOALY_H
#define GOALY_H
// 守门员派生类：在公共球员能力上增加守门决策与观察方向。原作者：Mat Buckland，2003（fup@ai-junkie.com）。
#include "Vector2D.h"
#include "EntityPlayer.h"
#include "GoalkeeperAI.h"
#include <memory>

class EntityPlayer;


class EntityPlayerGoalKeeper : public EntityPlayer
{
private:

   // 智能指针独占 AI 控制器；球员销毁时控制器及其状态自动销毁。
  std::unique_ptr<GoalkeeperAI> mAi;

  // 绘图时用此向量表现面向足球，移动方向仍由基类朝向决定。
  Vector2D   mLookAt;

public:

   EntityPlayerGoalKeeper(SoccerTeam*        homeTeam,
              int                homeRegion,
              GoalkeeperState startState,
              Vector2D           heading,
              Vector2D           velocity,
              double              mass,
              double              maxForce,
              double              maxSpeed,
              double              maxTurnRate,
              double              scale);

   ~EntityPlayerGoalKeeper();

   // 重写基类的更新、绘制和消息接口，提供守门员的具体行为。
   void        update();
   void        render();
   bool        handleMessage(const Telegram& msg);


   // 判断足球是否足够接近，使守门员可以考虑出击。
   bool        ballWithinRangeForIntercept()const;

   // 判断守门员是否离球门过远。
   bool        tooFarFromGoalMouth()const;

   // 根据足球纵向位置计算球门侧的阻挡目标，与足球一起决定守门员站位。
   Vector2D    getRearInterposeTarget()const;

   GoalkeeperAI* getAi()const;


   Vector2D    lookAt()const{return mLookAt;}
   void        setLookAt(Vector2D v){mLookAt=v;}
};



#endif
