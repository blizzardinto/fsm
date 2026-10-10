/*
 * 阅读提示：球员公共基类，在运动能力之上封装球队关系、角色、距离判断和移动行为。
 * 守门员与场上球员继承这些共同能力，同时分别提供自己的更新和 AI 实现。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "EntityPlayer.h"
#include "SteeringBehaviors.h"
#include "Transformations.h"
#include "Geometry.h"
#include "Cgdi.h"
#include "C2DMatrix.h"
#include "Region.h"
#include "ParamLoader.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"
#include "SoccerTeam.h"
#include "ParamLoader.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "SoccerPitch.h"
#include "DebugConsole.h"


using std::vector;


// 析构函数释放球员拥有的移动行为对象。
EntityPlayer::~EntityPlayer()
{
  delete mSteering;
}

// 构造函数建立球员初始位置、角色和移动参数。
EntityPlayer::EntityPlayer(SoccerTeam* homeTeam,
                       int   homeRegion,
                       Vector2D  heading,
                       Vector2D velocity,
                       double    mass,
                       double    maxForce,
                       double    maxSpeed,
                       double    maxTurnRate,
                       double    scale,
                       PlayerRole role):

    EntityMovable(homeTeam->pitch()->getRegionFromIndex(homeRegion)->center(),
                 scale*10.0,
                 velocity,
                 maxSpeed,
                 heading,
                 mass,
                 Vector2D(scale,scale),
                 maxTurnRate,
                 maxForce),
   mTeam(homeTeam),
   mDistSqToBall(maxFloat),
   mHomeRegion(homeRegion),
   mDefaultRegion(homeRegion),
   mPlayerRole(role)
{

  // 初始化球员轮廓顶点，并计算包围圆半径。
  const int numPlayerVerts = 4;
  const Vector2D player[numPlayerVerts] = {Vector2D(-3, 8),
                                            Vector2D(3,10),
                                            Vector2D(3,-10),
                                            Vector2D(-3,-8)};

  for (int vtx=0; vtx<numPlayerVerts; ++vtx)
  {
    mPlayerVertices.push_back(player[vtx]);

    // 包围半径取所有轮廓顶点到中心的最大距离。
    if (abs(player[vtx].x) > mBoundingRadius)
    {
      mBoundingRadius = abs(player[vtx].x);
    }

    if (abs(player[vtx].y) > mBoundingRadius)
    {
      mBoundingRadius = abs(player[vtx].y);
    }
  }

  // 为该球员创建独立的移动行为对象。
  mSteering = new SteeringBehaviors(this,
                                      mTeam->pitch(),
                                      ball());

  // 初始目标就是当前位置，因此球员开始时停留等待。
  mSteering->setTarget(homeTeam->pitch()->getRegionFromIndex(homeRegion)->center());
}




// 面向足球：根据足球位置更新朝向。
void EntityPlayer::trackBall()
{
  rotateHeadingToFacePosition(ball()->pos());
}

// 面向目标：根据移动目标更新朝向。
void EntityPlayer::trackTarget()
{
  setHeading(vec2DNormalize(steering()->target() - pos()));
}


// 排序比较函数接收两个球员指针，判断他们沿进攻方向的先后关系。
bool  sortByDistanceToOpponentsGoal(const EntityPlayer*const p1,
                                    const EntityPlayer*const p2)
{
  return (p1->distToOppGoal() < p2->distToOppGoal());
}

bool  sortByReversedDistanceToOpponentsGoal(const EntityPlayer*const p1,
                                            const EntityPlayer*const p2)
{
  return (p1->distToOppGoal() > p2->distToOppGoal());
}


// 判断目标是否位于球员正前方，用于视野与威胁判断。
bool EntityPlayer::positionInFrontOfPlayer(Vector2D position)const
{
  Vector2D toSubject = position - pos();

  if (toSubject.dot(heading()) > 0)

    return true;

  else

    return false;
}

// 判断前方是否有进入舒适距离的对手。
bool EntityPlayer::isThreatened()const
{
  // 遍历对方球员，检查近距离威胁。
  std::vector<EntityPlayer*>::const_iterator curOpp;
  curOpp = team()->opponents()->members().begin();

  for (curOpp; curOpp != team()->opponents()->members().end(); ++curOpp)
  {
    // 对手同时在前方且距离足够近时，认为受到威胁。
    if (positionInFrontOfPlayer((*curOpp)->pos()) &&
       (vec2DDistanceSq(pos(), (*curOpp)->pos()) < prm.playerComfortZoneSq))
    {
      return true;
    }

  }// 检查下一个对手。

  return false;
}

// 选择合适的支援球员，再通过消息通知他进入支援状态。
void EntityPlayer::findSupport()const
{
  // 尚无支援者时，为控球队员寻找支援者。
  if (team()->supportingPlayer() == NULL)
  {
    EntityPlayer* bestSupportPly = team()->determineBestSupportingAttacker();

    team()->setSupportingPlayer(bestSupportPly);

    dispatcher->dispatchMsg(sendMsgImmediately,
                            id(),
                            team()->supportingPlayer()->id(),
                            msgSupportAttacker,
                            NULL);
  }

  EntityPlayer* bestSupportPly = team()->determineBestSupportingAttacker();

  // 最佳支援者改变时，更新球队记录，并通知新旧支援者调整状态。
  if (bestSupportPly && (bestSupportPly != team()->supportingPlayer()))
  {

    if (team()->supportingPlayer())
    {
      dispatcher->dispatchMsg(sendMsgImmediately,
                              id(),
                              team()->supportingPlayer()->id(),
                              msgGoHome,
                              NULL);
    }



    team()->setSupportingPlayer(bestSupportPly);

    dispatcher->dispatchMsg(sendMsgImmediately,
                            id(),
                            team()->supportingPlayer()->id(),
                            msgSupportAttacker,
                            NULL);
  }
}


  // 计算到对方球门的距离，供传球策略使用。
double EntityPlayer::distToOppGoal()const
{
  return fabs(pos().x - team()->opponentsGoal()->center().x);
}

double EntityPlayer::distToHomeGoal()const
{
  return fabs(pos().x - team()->homeGoal()->center().x);
}

bool EntityPlayer::isControllingPlayer()const
{return team()->controllingPlayer()==this;}

bool EntityPlayer::ballWithinKeeperRange()const
{
  return (vec2DDistanceSq(pos(), ball()->pos()) < prm.keeperInBallRangeSq);
}

bool EntityPlayer::ballWithinReceivingRange()const
{
  return (vec2DDistanceSq(pos(), ball()->pos()) < prm.ballWithinReceivingRangeSq);
}

bool EntityPlayer::ballWithinKickingRange()const
{
  return (vec2DDistanceSq(ball()->pos(), pos()) < prm.playerKickingDistanceSq);
}


bool EntityPlayer::inHomeRegion()const
{
  if (mPlayerRole == goalKeeper)
  {
    return pitch()->getRegionFromIndex(mHomeRegion)->inside(pos(), Region::normal);
  }
  else
  {
    return pitch()->getRegionFromIndex(mHomeRegion)->inside(pos(), Region::halfsize);
  }
}

bool EntityPlayer::atTarget()const
{
  return (vec2DDistanceSq(pos(), steering()->target()) < prm.playerInTargetRangeSq);
}

bool EntityPlayer::isClosestTeamMemberToBall()const
{
  return team()->playerClosestToBall() == this;
}

bool EntityPlayer::isClosestPlayerOnPitchToBall()const
{
  return isClosestTeamMemberToBall() &&
         (distSqToBall() < team()->opponents()->closestDistToBallSq());
}

bool EntityPlayer::inHotRegion()const
{
  return fabs(pos().y - team()->opponentsGoal()->center().y ) <
         pitch()->playingArea()->length()/3.0;
}

bool EntityPlayer::isAheadOfAttacker()const
{
  return fabs(pos().x - team()->opponentsGoal()->center().x) <
         fabs(team()->controllingPlayer()->pos().x - team()->opponentsGoal()->center().x);
}

SoccerBall* const EntityPlayer::ball()const
{
  return team()->pitch()->ball();
}

SoccerPitch* const EntityPlayer::pitch()const
{
  return team()->pitch();
}

const Region* const EntityPlayer::homeRegion()const
{
  return pitch()->getRegionFromIndex(mHomeRegion);
}


