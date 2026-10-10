/*
 * 阅读提示：场上球员状态集合，分别处理追球、踢球、等待、带球、接球和支援。
 * 状态决定做什么，移动行为负责怎么算移动力，球员更新负责真正改变速度和位置。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "StatesPlayerOnField.h"
#include "DebugConsole.h"
#include "SoccerPitch.h"
#include "EntityPlayerOnField.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "geometry.h"
#include "SoccerBall.h"
#include "ParamLoader.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"

#include "Regulator.h"


// 启用场上球员的调试宏后，可以在调试窗口观察状态变化。
#define PLAYER_STATE_INFO_ON


// 全局状态：处理跨越具体动作的速度设置和公共消息。




void GlobalPlayerState::execute()
{
  // 从绑定的引用取得指针，只是为了沿用箭头调用写法，仍然操作同一个球员。
  auto* player = &mOwner;
  // 控球队员接近足球时降低最大速度，避免带球时跑得过快。
  if((player->ballWithinReceivingRange()) && (player->isControllingPlayer()))
  {
    player->setMaxSpeed(prm.playerMaxSpeedWithBall);
  }

  else
  {
     player->setMaxSpeed(prm.playerMaxSpeedWithoutBall);
  }

}


bool GlobalPlayerState::onMessage(const Telegram& telegram)
{
  auto* player = &mOwner;
  switch(telegram.msg)
  {
  case msgReceiveBall:
    {
      // 根据消息内容设置移动目标。
      player->steering()->setTarget(*(static_cast<Vector2D*>(telegram.extraInfo)));

      // 通过 AI 控制器切换状态。
      player->getAi()->changeState(FieldPlayerState::receiveBall);

      return true;
    }

    break;

  case msgSupportAttacker:
    {
      // 如果已经处于支援状态，就不重复切换。
      if (player->getAi()->isInState(FieldPlayerState::supportAttacker))
      {
        return true;
      }

      // 将目标设置为球队计算出的最佳支援位置。
      player->steering()->setTarget(player->team()->getSupportSpot());

      // 切换到消息要求的状态。
      player->getAi()->changeState(FieldPlayerState::supportAttacker);

      return true;
    }

    break;

 case msgWait:
    {
      // 切换到消息要求的状态。
      player->getAi()->changeState(FieldPlayerState::wait);

      return true;
    }

    break;

  case msgGoHome:
    {
      player->setDefaultHomeRegion();

      player->getAi()->changeState(FieldPlayerState::returnToHomeRegion);

      return true;
    }

    break;

  case msgPassToMe:
    {

      // 取出请求传球的队员对象，读取他的位置。
      EntityPlayerOnField* receiver = static_cast<EntityPlayerOnField*>(telegram.extraInfo);

      #ifdef PLAYER_STATE_INFO_ON
      debugCon << "Player " << player->id() << " received request from " <<
                    receiver->id() << " to make pass" << "";
      #endif

      // 足球不在踢球范围，或已有接球者时，不能响应新的传球请求。
      if (player->team()->receiver() != NULL ||
         !player->ballWithinKickingRange() )
      {
        #ifdef PLAYER_STATE_INFO_ON
        debugCon << "Player " << player->id() << " cannot make requested pass <cannot kick ball>" << "";
        #endif

        return true;
      }

      // 朝请求者的位置传球。
      player->ball()->kick(receiver->pos() - player->ball()->pos(),
                           prm.maxPassingForce);


     #ifdef PLAYER_STATE_INFO_ON
     debugCon << "Player " << player->id() << " Passed ball to requesting player" << "";
     #endif

      // 通知队友进入接球状态。
      Vector2D passTarget = receiver->pos();
      dispatcher->dispatchMsg(sendMsgImmediately,
                              player->id(),
                              receiver->id(),
                              msgReceiveBall,
                              &passTarget);



      // 传球后切换状态。
      player->getAi()->changeState(FieldPlayerState::wait);

      player->findSupport();

      return true;
    }

    break;

  }// 结束消息分支。

  return false;
}




// 追球状态：接近足球，准备踢球。




void ChaseBall::enter()
{
  auto* player = &mOwner;
  player->steering()->seekOn();

  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters chase state" << "";
  #endif
}

void ChaseBall::execute()
{
  auto* player = &mOwner;
  // 足球进入踢球范围时，切换到踢球状态。
  if (player->ballWithinKickingRange())
  {
    player->getAi()->changeState(FieldPlayerState::kickBall);

    return;
  }

  // 本队离球最近的球员继续追球。
  if (player->isClosestTeamMemberToBall())
  {
    player->steering()->setTarget(player->ball()->pos());

    return;
  }

  // 如果不再是最近的球员，就回到自己的区域等待机会。
  player->getAi()->changeState(FieldPlayerState::returnToHomeRegion);
}


void ChaseBall::exit()
{
  auto* player = &mOwner;
  player->steering()->seekOff();
}



// 支援状态：为控球队员提供可传球的位置。




void SupportAttacker::enter()
{
  auto* player = &mOwner;
  player->steering()->arriveOn();

  player->steering()->setTarget(player->team()->getSupportSpot());

  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters support state" << "";
  #endif
}

void SupportAttacker::execute()
{
  auto* player = &mOwner;
  // 本队失去控球权后，返回自己的区域。
  if (!player->team()->inControl())
  {
    player->getAi()->changeState(FieldPlayerState::returnToHomeRegion); return;
  }


  // 最佳支援位置变化时，同步更新移动目标。
  if (player->team()->getSupportSpot() != player->steering()->target())
  {
    player->steering()->setTarget(player->team()->getSupportSpot());

    player->steering()->arriveOn();
  }

  // 如果自己具备射门机会，就向控球队员请求传球。
  if( player->team()->canShoot(player->pos(),
                               prm.maxShootingForce))
  {
    player->team()->requestPass(player);
  }


  // 已到支援点且仍由本队控球时，停下并面向足球。
  if (player->atTarget())
  {
    player->steering()->arriveOff();

    // 面向足球，保持观察方向。
    player->trackBall();

    player->setVelocity(Vector2D(0,0));

    // 附近没有对手威胁时，请求传球。
    if (!player->isThreatened())
    {
      player->team()->requestPass(player);
    }
  }
}


void SupportAttacker::exit()
{
  auto* player = &mOwner;
  // 清空球队的支援球员记录，便于重新选择支援者。
  player->team()->setSupportingPlayer(NULL);

  player->steering()->arriveOff();
}




// 返回区域状态：让球员回到分配给他的站位区域。




void ReturnToHomeRegion::enter()
{
  auto* player = &mOwner;
  player->steering()->arriveOn();

  if (!player->homeRegion()->inside(player->steering()->target(), Region::halfsize))
  {
    player->steering()->setTarget(player->homeRegion()->center());
  }

  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters ReturnToHome state" << "";
  #endif
}

void ReturnToHomeRegion::execute()
{
  auto* player = &mOwner;
  if (player->pitch()->gameOn())
  {
    // 自己是本队最近球员，且没有接球者或持球守门员时，转去追球。
    if ( player->isClosestTeamMemberToBall() &&
         (player->team()->receiver() == NULL) &&
         !player->pitch()->entityPlayerGoalKeeperHasBall())
    {
      player->getAi()->changeState(FieldPlayerState::chaseBall);

      return;
    }
  }

  // 比赛进行中，接近区域后进入等待状态，并把当前位置设为目标，便于被挤开后返回。
  if (player->pitch()->gameOn() && player->homeRegion()->inside(player->pos(),
                                                             Region::halfsize))
  {
    player->steering()->setTarget(player->pos());
    player->getAi()->changeState(FieldPlayerState::wait);
  }
  // 比赛暂停时，需要更接近区域中心才能结束返回动作。
  else if(!player->pitch()->gameOn() && player->atTarget())
  {
    player->getAi()->changeState(FieldPlayerState::wait);
  }
}

void ReturnToHomeRegion::exit()
{
  auto* player = &mOwner;
  player->steering()->arriveOff();
}




// 等待状态：保持站位，同时观察是否需要接球或追球。




void Wait::enter()
{
  auto* player = &mOwner;
  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters wait state" << "";
  #endif

  // 开球前将目标设为区域中心，使球员回到正确站位。
  if (!player->pitch()->gameOn())
  {
    player->steering()->setTarget(player->homeRegion()->center());
  }
}

void Wait::execute()
{
  auto* player = &mOwner;
  // 如果被其他球员挤离目标位置，就移动回去。
  if (!player->atTarget())
  {
    player->steering()->arriveOn();

    return;
  }

  else
  {
    player->steering()->arriveOff();

    player->setVelocity(Vector2D(0,0));

    // 面向足球，保持观察方向。
    player->trackBall();
  }

  // 本队控球，自己不是控球队员且位置更靠前时，请求传球。
  if ( player->team()->inControl()    &&
     (!player->isControllingPlayer()) &&
       player->isAheadOfAttacker() )
  {
    player->team()->requestPass(player);

    return;
  }

  if (player->pitch()->gameOn())
  {
   // 自己是本队最近球员，没有接球者且守门员未持球时，开始追球。
   if (player->isClosestTeamMemberToBall() &&
       player->team()->receiver() == NULL  &&
       !player->pitch()->entityPlayerGoalKeeperHasBall())
   {
     player->getAi()->changeState(FieldPlayerState::chaseBall);

     return;
   }
  }
}

void Wait::exit()
{
  auto* player = &mOwner;}




// 踢球状态：依次考虑射门、传球和带球。




void KickBall::enter()
{
  auto* player = &mOwner;
  // 向球队登记当前控球队员。
   player->team()->setControllingPlayer(player);

   // 调节器限制踢球频率，避免每次更新都能踢球。
   if (!player->isReadyForNextKick())
   {
     player->getAi()->changeState(FieldPlayerState::chaseBall);
   }


  #ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters kick state" << "";
  #endif
}

void KickBall::execute()
{
  auto* player = &mOwner;
  // 用点积判断足球方向与球员朝向是否一致。
  Vector2D toBall = player->ball()->pos() - player->pos();
  double   dot    = player->heading().dot(vec2DNormalize(toBall));

  // 守门员持球、足球在身后或已有接球者时，继续追球而不踢球。
  if (player->team()->receiver() != NULL   ||
      player->pitch()->entityPlayerGoalKeeperHasBall() ||
      (dot < 0) )
  {
    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Goaly has ball / ball behind player" << "";
    #endif

    player->getAi()->changeState(FieldPlayerState::chaseBall);

    return;
  }

  /* 尝试射门。 */

  // 如果能射门，这个向量保存对方门线上应瞄准的位置。
  Vector2D    ballTarget;

  // 足球越接近正前方，允许使用的射门力量越大。
  double power = prm.maxShootingForce * dot;

  // 能够射门，或随机决定尝试射门时，执行射门动作。
  if (player->team()->canShoot(player->ball()->pos(),
                               power,
                               ballTarget)                   ||
     (randFloat() < prm.chancePlayerAttemptsPotShot))
  {
   #ifdef PLAYER_STATE_INFO_ON
   debugCon << "Player " << player->id() << " attempts a shot at " << ballTarget << "";
   #endif

   // 加入方向误差，模拟踢球精度；误差大小由 playerKickingAccuracy 控制。
   ballTarget = addNoiseToKick(player->ball()->pos(), ballTarget);

   // 根据足球位置和瞄准位置计算踢球方向。
   Vector2D kickDirection = ballTarget - player->ball()->pos();

   player->ball()->kick(kickDirection, power);

   // 踢球后切换状态。
   player->getAi()->changeState(FieldPlayerState::wait);

   player->findSupport();

   return;
 }


  /* 尝试传球。 */

  // 找到接球者后，这个指针指向该队友；它不负责销毁队友。
  EntityPlayer* receiver = NULL;

  power = prm.maxPassingForce * dot;

  // 检查是否存在合适的接球队员。
  if (player->isThreatened()  &&
      player->team()->findPass(player,
                              receiver,
                              ballTarget,
                              power,
                              prm.minPassDist))
  {
    // 为传球方向加入精度误差。
    ballTarget = addNoiseToKick(player->ball()->pos(), ballTarget);

    Vector2D kickDirection = ballTarget - player->ball()->pos();

    player->ball()->kick(kickDirection, power);

    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Player " << player->id() << " passes the ball with force " << power << "  to player "
              << receiver->id() << "  Target is " << ballTarget << "";
    #endif


    // 发送消息通知队友准备接球。
    dispatcher->dispatchMsg(sendMsgImmediately,
                            player->id(),
                            receiver->id(),
                            msgReceiveBall,
                            &ballTarget);


    // 传球后留在当前位置等待后续指令。
    player->getAi()->changeState(FieldPlayerState::wait);

    player->findSupport();

    return;
  }

  // 无法射门或传球时，改为带球推进。
  else
  {
    player->findSupport();

    player->getAi()->changeState(FieldPlayerState::dribble);
  }
}


// 带球状态：通过短距离踢球逐步推进。




void Dribble::enter()
{
  auto* player = &mOwner;
  // 向球队登记当前控球队员。
  player->team()->setControllingPlayer(player);

#ifdef PLAYER_STATE_INFO_ON
  debugCon << "Player " << player->id() << " enters dribble state" << "";
  #endif
}

void Dribble::execute()
{
  auto* player = &mOwner;
  double dot = player->team()->homeGoal()->facing().dot(player->heading());

  // 朝向不利于向前推进时，通过小角度转向和轻踢来调整足球方向。
  if (dot < 0)
  {
    // 将朝向旋转一小段角度，再朝该方向轻踢足球。
    Vector2D direction = player->heading();

    // 根据朝向与球门方向的叉积符号，选择顺时针或逆时针转向。
    double angle = quarterPi * -1 *
                 player->team()->homeGoal()->facing().sign(player->heading());

    vec2DRotateAroundOrigin(direction, angle);

    // 调整方向时使用较小力量，便于控制足球。
    const double kickingForce = 0.8;

    player->ball()->kick(direction, kickingForce);
  }

  // 朝对方半场踢球推进。
  else
  {
    player->ball()->kick(player->team()->homeGoal()->facing(),
                         prm.maxDribbleForce);
  }

  // 球已踢出，切换到追球状态跟上它。
  player->getAi()->changeState(FieldPlayerState::chaseBall);

  return;
}



// 接球状态：选择到达目标点或追踪足球的移动方式。




void ReceiveBall::enter()
{
  auto* player = &mOwner;
  // 向球队登记接球队员。
  player->team()->setReceiver(player);

  // 同时把接球队员登记为控球队员。
  player->team()->setControllingPlayer(player);

  // 接球有两种策略：减速到达传球目标，或追踪移动的足球；根据随机概率、对手距离和进攻区域选择。
  const double passThreatRadius = 70.0;

  if (( player->inHotRegion() ||
        randFloat() < prm.chanceOfUsingArriveTypeReceiveBehavior) &&
     !player->team()->isOpponentWithinRadius(player->pos(), passThreatRadius))
  {
    player->steering()->arriveOn();

    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Player " << player->id() << " enters receive state (Using Arrive)" << "";
    #endif
  }
  else
  {
    player->steering()->pursuitOn();

    #ifdef PLAYER_STATE_INFO_ON
    debugCon << "Player " << player->id() << " enters receive state (Using Pursuit)" << "";
    #endif
  }
}

void ReceiveBall::execute()
{
  auto* player = &mOwner;
  // 足球足够近，或本队失去控球权时，切换到追球状态。
  if (player->ballWithinReceivingRange() || !player->team()->inControl())
  {
    player->getAi()->changeState(FieldPlayerState::chaseBall);

    return;
  }

  if (player->steering()->pursuitIsOn())
  {
    player->steering()->setTarget(player->ball()->pos());
  }

  // 已到移动目标时停下，并转向足球。
  if (player->atTarget())
  {
    player->steering()->arriveOff();
    player->steering()->pursuitOff();
    player->trackBall();
    player->setVelocity(Vector2D(0,0));
  }
}

void ReceiveBall::exit()
{
  auto* player = &mOwner;
  player->steering()->arriveOff();
  player->steering()->pursuitOff();

  player->team()->setReceiver(NULL);
}









GlobalPlayerState::GlobalPlayerState(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* GlobalPlayerState::name() const { return "GlobalPlayerState"; }

void GlobalPlayerState::enter() {}

void GlobalPlayerState::exit() {}


ChaseBall::ChaseBall(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* ChaseBall::name() const { return "ChaseBall"; }

bool ChaseBall::onMessage(const Telegram& telegram) {return false;}


Dribble::Dribble(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* Dribble::name() const { return "Dribble"; }

void Dribble::exit() {}

bool Dribble::onMessage(const Telegram& telegram) {return false;}


ReturnToHomeRegion::ReturnToHomeRegion(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* ReturnToHomeRegion::name() const { return "ReturnToHomeRegion"; }

bool ReturnToHomeRegion::onMessage(const Telegram& telegram) {return false;}


Wait::Wait(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* Wait::name() const { return "Wait"; }

bool Wait::onMessage(const Telegram& telegram) {return false;}


KickBall::KickBall(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* KickBall::name() const { return "KickBall"; }

void KickBall::exit() {}

bool KickBall::onMessage(const Telegram& telegram) {return false;}


ReceiveBall::ReceiveBall(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* ReceiveBall::name() const { return "ReceiveBall"; }

bool ReceiveBall::onMessage(const Telegram& telegram) {return false;}


SupportAttacker::SupportAttacker(EntityPlayerOnField& owner) : mOwner(owner) {}

const char* SupportAttacker::name() const { return "SupportAttacker"; }

bool SupportAttacker::onMessage(const Telegram& telegram) {return false;}

