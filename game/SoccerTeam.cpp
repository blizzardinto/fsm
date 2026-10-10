/*
 * 阅读提示：球队聚合对象，拥有球员、球队 AI 和支援位置计算器。
 * 它还借用球场、球门及对手指针；读成员时先区分拥有关系与临时协作关系，才能理解生命周期。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "EntityPlayer.h"
#include "EntityPlayerGoalKeeper.h"
#include "EntityPlayerOnField.h"
#include "utils.h"
#include "SteeringBehaviors.h"
#include "ParamLoader.h"
#include "geometry.h"
#include "EntityManager.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"
#include "DebugConsole.h"
#include <windows.h>

using std::vector;


// 球队构造函数建立阵型、AI、球员和支援位置计算器。
SoccerTeam::SoccerTeam(Goal*        homeGoal,
                       Goal*        opponentsGoal,
                       SoccerPitch* pitch,
                       TeamColor   color):mOpponentsGoal(opponentsGoal),
                                           mHomeGoal(homeGoal),
                                           mOpponents(NULL),
                                           mPitch(pitch),
                                           mColor(color),
                                           mDistSqToBallOfClosestPlayer(0.0),
                                           mSupportingPlayer(NULL),
                                           mReceivingPlayer(NULL),
                                           mControllingPlayer(NULL),
                                           mPlayerClosestToBall(NULL)
{
  mAi = std::make_unique<TeamAI>(*this);
  // 保留原来的初始防守设置：此时球员尚未创建，不执行依赖球员的进入回调。
  mAi->initialize(TeamState::defending);

  // 创建场上球员和守门员。
  createPlayers();

  // 设置球员的默认移动行为。
  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    (*it)->steering()->separationOn();
  }

  // 为球队创建支援位置计算器。
  mSupportSpotCalc = new SupportSpotCalculator(prm.numSupportSpotsX,
                                                 prm.numSupportSpotsY,
                                                 this);
}

// 球队析构函数释放拥有的球员和支援位置计算器。
SoccerTeam::~SoccerTeam()
{

  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();
  for (it; it != mPlayers.end(); ++it)
  {
    delete *it;
  }

  delete mSupportSpotCalc;
}

// 球队每轮更新先计算共享数据，再运行球队 AI，最后更新各球员。
void SoccerTeam::update()
{
  // 最近球员信息在多个决策中复用，因此每轮只计算一次。
  calculateClosestPlayerToBall();

  // 球队 AI 在进攻、防守和开球准备之间切换，管理球队层面的策略。
  mAi->update();

  // 通过基类指针逐个更新球员，虚函数会调用各角色的实际实现。
  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    (*it)->update();
  }

}


// 找出本队距离足球最近的球员，并保存其指针。
void SoccerTeam::calculateClosestPlayerToBall()
{
  double closestSoFar = maxFloat;

  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    // 比较距离平方，避免开平方。
    double dist = vec2DDistanceSq((*it)->pos(), pitch()->ball()->pos());

    // 缓存每位球员到球的距离平方。
    (*it)->setDistSqToBall(dist);

    if (dist < closestSoFar)
    {
      closestSoFar = dist;

      mPlayerClosestToBall = *it;
    }
  }

  mDistSqToBallOfClosestPlayer = closestSoFar;
}


// 从可支援的球员中选择离最佳支援点最近的人。
EntityPlayer* SoccerTeam::determineBestSupportingAttacker()
{
  double closestSoFar = maxFloat;

  EntityPlayer* bestPlayer = NULL;

  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    // 只有进攻角色参与最佳支援者的选择。
    if ( ((*it)->role() == EntityPlayer::attacker) && ((*it) != mControllingPlayer) )
    {
      // 用距离平方比较远近。
      double dist = vec2DDistanceSq((*it)->pos(), mSupportSpotCalc->getBestSupportingSpot());

      // 排除控球队员，在合适角色中保留离支援点最近的人。
      if ((dist < closestSoFar) )
      {
        closestSoFar = dist;

        bestPlayer = (*it);
      }
    }
  }

  return bestPlayer;
}

// 寻找安全且更接近对方球门的传球；结果通过接球者和目标位置的引用参数返回。
bool SoccerTeam::findPass(const EntityPlayer*const passer,
                         EntityPlayer*&           receiver,
                         Vector2D&              passTarget,
                         double                  power,
                         double                  minPassingDistance)const
{

  std::vector<EntityPlayer*>::const_iterator curPlyr = members().begin();

  double    closestToGoalSoFar = maxFloat;
  Vector2D target;

  // 遍历本队成员，评估潜在接球者。
  for (curPlyr; curPlyr != members().end(); ++curPlyr)
  {
    // 不向自己传球，并要求接球者超过最小传球距离。
    if ( (*curPlyr != passer) &&
        (vec2DDistanceSq(passer->pos(), (*curPlyr)->pos()) >
         minPassingDistance*minPassingDistance))
    {
      if (getBestPassToReceiver(passer, *curPlyr, target, power))
      {
        // 目标比已有方案更接近对方门线时，保存这个方案。
        double dist2Goal = fabs(target.x - opponentsGoal()->center().x);

        if (dist2Goal < closestToGoalSoFar)
        {
          closestToGoalSoFar = dist2Goal;

          // 记录接球队员的借用指针。
          receiver = *curPlyr;

          // 记录传球目标位置。
          passTarget = target;
        }
      }
    }
  }// 检查下一个队友。

  if (receiver) return true;

  else return false;
}


// 对每个接球者评估三个目标：当前位置和可达圆的两个切点；只保留场内且安全的目标，再选最靠近对方门线的方案。
bool SoccerTeam::getBestPassToReceiver(const EntityPlayer* const passer,
                                       const EntityPlayer* const receiver,
                                       Vector2D&               passTarget,
                                       double                   power)const
{
  // 先假设接球者不动，估算足球到达他当前位置所需时间。
  double time = pitch()->ball()->timeToCoverDistance(pitch()->ball()->pos(),
                                                    receiver->pos(),
                                                    power);

  // 给定力量无法让足球到达接球者时，方案无效。
  if (time < 0) return false;

  // 根据时间和最大速度估算接球者能移动多远。
  double interceptRange = time * receiver->maxSpeed();

  // 缩小可拦截范围，为估算留出余量。
  const double scalingFactor = 0.3;
  interceptRange *= scalingFactor;

  // 从球位置向接球者可达圆作切线，计算两个候选传球目标。
  Vector2D ip1, ip2;

  getTangentPoints(receiver->pos(),
                   interceptRange,
                   pitch()->ball()->pos(),
                   ip1,
                   ip2);

  const int numPassesToTry = 3;
  Vector2D passes[numPassesToTry] = {ip1, receiver->pos(), ip2};


  // 候选目标必须在场内、不会被对手拦截，并且比已有方案更靠近对方门线。

  double closestSoFar = maxFloat;
  bool  bResult      = false;

  for (int pass=0; pass<numPassesToTry; ++pass)
  {
    double dist = fabs(passes[pass].x - opponentsGoal()->center().x);

    if (( dist < closestSoFar) &&
        pitch()->playingArea()->inside(passes[pass]) &&
        isPassSafeFromAllOpponents(pitch()->ball()->pos(),
                                   passes[pass],
                                   receiver,
                                   power))

    {
      closestSoFar = dist;
      passTarget   = passes[pass];
      bResult      = true;
    }
  }

  return bResult;
}

// 检查单个对手是否能拦截这次传球。
bool SoccerTeam::isPassSafeFromOpponent(Vector2D    from,
                                        Vector2D    target,
                                        const EntityPlayer* const receiver,
                                        const EntityPlayer* const opp,
                                        double       passingForce)const
{
  // 转入以传球方向为横轴的局部坐标，简化距离判断。
  Vector2D toTarget = target - from;
  Vector2D toTargetNormalized = vec2DNormalize(toTarget);

  Vector2D localPosOpp = pointToLocalSpace(opp->pos(),
                                         toTargetNormalized,
                                         toTargetNormalized.perp(),
                                         from);

  // 对手在传球者身后时视为安全；这里假设足球比对手跑得快。
  if ( localPosOpp.x < 0 )
  {
    return true;
  }

  // 对手位于目标更远处时，比较他与接球者到目标的距离。
  if (vec2DDistanceSq(from, target) < vec2DDistanceSq(opp->pos(), from))
  {
    if (receiver)
    {
      if ( vec2DDistanceSq(target, opp->pos())  >
           vec2DDistanceSq(target, receiver->pos()) )
      {
        return true;
      }

      else
      {
        return false;
      }

    }

    else
    {
      return true;
    }
  }

  // 估算足球到达对手在传球轴上的投影位置所需时间。
  double timeForBall =
  pitch()->ball()->timeToCoverDistance(Vector2D(0,0),
                                       Vector2D(localPosOpp.x, 0),
                                       passingForce);

  // 根据该时间估算对手最多能跑多远。
  double reach = opp->maxSpeed() * timeForBall +
                pitch()->ball()->boundingRadius()+
                opp->boundingRadius();

  // 如果对手到传球线的距离不超过可跑距离加双方半径，就可能拦截足球。
  if ( fabs(localPosOpp.y) < reach )
  {
    return false;
  }

  return true;
}

// 对所有对方球员逐一检查；全部不能拦截时，才认为传球安全。
bool SoccerTeam::isPassSafeFromAllOpponents(Vector2D                from,
                                            Vector2D                target,
                                            const EntityPlayer* const receiver,
                                            double     passingForce)const
{
  std::vector<EntityPlayer*>::const_iterator opp = opponents()->members().begin();

  for (opp; opp != opponents()->members().end(); ++opp)
  {
    if (!isPassSafeFromOpponent(from, target, receiver, *opp, passingForce))
    {
      debugOn

      return false;
    }
  }

  return true;
}

// 在对方门线上随机尝试射门目标，检查力量是否足够及路线是否安全；成功时通过引用返回目标位置。
bool SoccerTeam::canShoot(Vector2D  ballPos,
                          double     power,
                          Vector2D  shotTarget)const
{
  // 本次最多尝试的随机射门目标数。
  int numAttempts = prm.numAttemptsToFindValidStrike;

  while (numAttempts--)
  {
    // 在两门柱之间随机选点，并为足球半径留出空间。
    shotTarget = opponentsGoal()->center();

    // 目标纵坐标不能太靠近门柱，否则足球圆形轮廓会撞柱。
    int minYVal = opponentsGoal()->leftPost().y + pitch()->ball()->boundingRadius();
    int maxYVal = opponentsGoal()->rightPost().y - pitch()->ball()->boundingRadius();

    shotTarget.y = (double)randInt(minYVal, maxYVal);

    // 检查给定力量是否足以让足球到达门线。
    double time = pitch()->ball()->timeToCoverDistance(ballPos,
                                                      shotTarget,
                                                      power);

    // 足球能到达目标后，再检查对手是否能拦截。
    if (time >= 0)
    {
      if (isPassSafeFromAllOpponents(ballPos, shotTarget, NULL, power))
      {
        return true;
      }
    }
  }

  return false;
}


// 向所有场上球员发送回到站位区域的消息。
void SoccerTeam::returnAllEntityPlayerOnFieldsToHome()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ((*it)->role() != EntityPlayer::goalKeeper)
    {
      dispatcher->dispatchMsg(sendMsgImmediately,
                            1,
                            (*it)->id(),
                            msgGoHome,
                            NULL);
    }
  }
}


// 绘制球队球员及球队层面的调试信息。
void SoccerTeam::render()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    (*it)->render();
  }

  // 在画面上方显示控球队伍和球员编号。
  if (prm.bShowControllingTeam)
  {
    gdi->textColor(Cgdi::white);

    if ( (color() == blue) && inControl())
    {
      gdi->textAtPos(20,3,"Blue in Control");
    }
    else if ( (color() == red) && inControl())
    {
      gdi->textAtPos(20,3,"Red in Control");
    }
    if (mControllingPlayer != NULL)
    {
      gdi->textAtPos(pitch()->cxClient()-150, 3, "Controlling Player: " + ttos(mControllingPlayer->id()));
    }
  }

  // 绘制支援位置及评分。
  if (prm.bSupportSpots && inControl())
  {
    mSupportSpotCalc->render();
  }

// 启用 SHOW_TEAM_STATE 宏可显示球队状态。
#ifdef SHOW_TEAM_STATE
  if (color() == red)
  {
    gdi->textColor(Cgdi::white);

    if (mAi->isInState(TeamState::attacking))
    {
      gdi->textAtPos(160, 20, "Attacking");
    }
    if (mAi->isInState(TeamState::defending))
    {
      gdi->textAtPos(160, 20, "Defending");
    }
    if (mAi->isInState(TeamState::prepareForKickOff))
    {
      gdi->textAtPos(160, 20, "Kickoff");
    }
  }
  else
  {
    if (mAi->isInState(TeamState::attacking))
    {
      gdi->textAtPos(160, pitch()->cyClient()-40, "Attacking");
    }
    if (mAi->isInState(TeamState::defending))
    {
      gdi->textAtPos(160, pitch()->cyClient()-40, "Defending");
    }
    if (mAi->isInState(TeamState::prepareForKickOff))
    {
      gdi->textAtPos(160, pitch()->cyClient()-40, "Kickoff");
    }
  }
#endif

// 启用 SHOW_SUPPORTING_PLAYERS_TARGET 宏可显示支援目标。
#ifdef SHOW_SUPPORTING_PLAYERS_TARGET
  if (mSupportingPlayer)
  {
    gdi->blueBrush();
    gdi->redPen();
    gdi->circle(mSupportingPlayer->steering()->target(), 4);

  }
#endif

}

// 根据球队颜色创建对应的守门员和场上球员。
void SoccerTeam::createPlayers()
{
  if (color() == blue)
  {
    // 创建守门员。
    mPlayers.push_back(new EntityPlayerGoalKeeper(this,
                               1,
                               GoalkeeperState::tendGoal,
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale));

    // 创建场上球员。
    mPlayers.push_back(new EntityPlayerOnField(this,
                               6,
                               FieldPlayerState::wait,
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));



        mPlayers.push_back(new EntityPlayerOnField(this,
                               8,
                               FieldPlayerState::wait,
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));





        mPlayers.push_back(new EntityPlayerOnField(this,
                               3,
                               FieldPlayerState::wait,
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::defender));


        mPlayers.push_back(new EntityPlayerOnField(this,
                               5,
                               FieldPlayerState::wait,
                               Vector2D(0,1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                              EntityPlayer::defender));

  }

  else
  {

     // 创建守门员。
    mPlayers.push_back(new EntityPlayerGoalKeeper(this,
                               16,
                               GoalkeeperState::tendGoal,
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale));


    // 创建场上球员。
    mPlayers.push_back(new EntityPlayerOnField(this,
                               9,
                               FieldPlayerState::wait,
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));

    mPlayers.push_back(new EntityPlayerOnField(this,
                               11,
                               FieldPlayerState::wait,
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::attacker));



    mPlayers.push_back(new EntityPlayerOnField(this,
                               12,
                               FieldPlayerState::wait,
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::defender));


    mPlayers.push_back(new EntityPlayerOnField(this,
                               14,
                               FieldPlayerState::wait,
                               Vector2D(0,-1),
                               Vector2D(0.0, 0.0),
                               prm.playerMass,
                               prm.playerMaxForce,
                               prm.playerMaxSpeedWithoutBall,
                               prm.playerMaxTurnRate,
                               prm.playerScale,
                               EntityPlayer::defender));

  }

  // 将球员登记到实体管理器，供消息系统按编号查找。
  std::vector<EntityPlayer*>::iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    entityMgr->registerEntity(*it);
  }
}


EntityPlayer* SoccerTeam::getPlayerFromId(int id)const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ((*it)->id() == id) return *it;
  }

  return NULL;
}


void SoccerTeam::setPlayerHomeRegion(int plyr, int region)const
{
  assert ( (plyr>=0) && (plyr<mPlayers.size()) );

  mPlayers[plyr]->setHomeRegion(region);
}


// 阵型变化后，更新等待或返回中的球员目标。
void SoccerTeam::updateTargetsOfWaitingPlayers()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ( (*it)->role() != EntityPlayer::goalKeeper )
    {
      // 已确认不是守门员后，转换为场上球员指针以访问专属 AI 接口。
      EntityPlayerOnField* plyr = static_cast<EntityPlayerOnField*>(*it);

      if ( plyr->getAi()->isInState(FieldPlayerState::wait) ||
           plyr->getAi()->isInState(FieldPlayerState::returnToHomeRegion) )
      {
        plyr->steering()->setTarget(plyr->homeRegion()->center());
      }
    }
  }
}


// 所有球员都在自己的区域内时返回真。
bool SoccerTeam::allPlayersAtHome()const
{
  std::vector<EntityPlayer*>::const_iterator it = mPlayers.begin();

  for (it; it != mPlayers.end(); ++it)
  {
    if ((*it)->inHomeRegion() == false)
    {
      return false;
    }
  }

  return true;
}

// 判断请求者能否安全接球，可以时向控球队员发送传球请求。
void SoccerTeam::requestPass(EntityPlayerOnField* requester)const
{
  // 用随机概率减少过于频繁的传球请求。
  if (randFloat() > 0.1) return;

  if (isPassSafeFromAllOpponents(controllingPlayer()->pos(),
                                 requester->pos(),
                                 requester,
                                 prm.maxPassingForce))
  {

    // 通知控球队员把球传给请求者；接球通知由传球动作发送。
    dispatcher->dispatchMsg(sendMsgImmediately,
                          requester->id(),
                          controllingPlayer()->id(),
                          msgPassToMe,
                          requester);

  }
}


// 检查指定位置附近是否存在对方球员。
bool SoccerTeam::isOpponentWithinRadius(Vector2D pos, double rad)
{
  std::vector<EntityPlayer*>::const_iterator end = opponents()->members().end();
  std::vector<EntityPlayer*>::const_iterator it;

  for (it=opponents()->members().begin(); it !=end; ++it)
  {
    if (vec2DDistanceSq(pos, (*it)->pos()) < rad*rad)
    {
      return true;
    }
  }

  return false;
}

TeamAI* SoccerTeam::getAi()const { return mAi.get(); }
