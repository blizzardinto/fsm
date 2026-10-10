/*
 * 阅读提示：球队状态集合。每个类代表一套球队策略，并在构造时绑定同一个所属球队。
 * 把变化的策略拆成状态类后，球队不需要在更新函数里写一个庞大的条件分支。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "StatesTeam.h"
#include "SoccerTeam.h"
#include "EntityPlayer.h"
#include "MessageDispatcher.h"
#include "SoccerMessages.h"
#include "constants.h"
#include "SoccerPitch.h"

// 启用 DEBUG_TEAM_STATES 调试宏，可在调试窗口查看球队状态。
#include "DebugConsole.h"




void changePlayerHomeRegions(SoccerTeam* team, const int newRegions[teamSize])
{
  for (int plyr=0; plyr<teamSize; ++plyr)
  {
    team->setPlayerHomeRegion(plyr, newRegions[plyr]);
  }
}

// 进攻状态：调整阵型并持续选择支援位置。




void Attacking::enter()
{
  // 状态操作绑定的球队对象；球队内部再协调自己的球员。
  auto* team = &mOwner;
#ifdef DEBUG_TEAM_STATES
  debugCon << team->name() << " entering Attacking state" << "";
#endif

  // 为不同颜色的球队设置进攻阵型区域编号。
  const int blueRegions[teamSize] = {1,12,14,6,4};
  const int redRegions[teamSize] = {16,3,5,9,13};

  // 根据球队颜色，把各球员分配到对应的区域。
  if (team->color() == SoccerTeam::blue)
  {
    changePlayerHomeRegions(team, blueRegions);
  }
  else
  {
    changePlayerHomeRegions(team, redRegions);
  }

  // 等待或返回区域的球员需要同步更新移动目标，才能前往新的站位。
  team->updateTargetsOfWaitingPlayers();
}


void Attacking::execute()
{
  auto* team = &mOwner;
  // 失去控球权后切换到防守状态。
  if (!team->inControl())
  {
    team->getAi()->changeState(TeamState::defending); return;
  }

  // 计算最适合支援进攻的位置。
  team->determineBestSupportingPosition();
}

void Attacking::exit()
{
  auto* team = &mOwner;
  // 离开进攻状态时清空支援球员记录。
  team->setSupportingPlayer(NULL);
}



// 防守状态：安排防守阵型。



void Defending::enter()
{
  auto* team = &mOwner;
#ifdef DEBUG_TEAM_STATES
  debugCon << team->name() << " entering Defending state" << "";
#endif

  // 为不同颜色的球队设置防守阵型区域编号。
  const int blueRegions[teamSize] = {1,6,8,3,5};
  const int redRegions[teamSize] = {16,9,11,12,14};

  // 根据球队颜色更新每个球员的区域。
  if (team->color() == SoccerTeam::blue)
  {
    changePlayerHomeRegions(team, blueRegions);
  }
  else
  {
    changePlayerHomeRegions(team, redRegions);
  }

  // 同步等待和返回球员的移动目标。
  team->updateTargetsOfWaitingPlayers();
}

void Defending::execute()
{
  auto* team = &mOwner;
  // 获得控球权后切换到进攻状态。
  if (team->inControl())
  {
    team->getAi()->changeState(TeamState::attacking); return;
  }
}


void Defending::exit()
{
  auto* team = &mOwner;}


// 开球准备状态：清空上一轮协作关系，让双方回到站位。


void PrepareForKickOff::enter()
{
  auto* team = &mOwner;
  // 重置控球、支援、接球等关键球员指针。
  team->setControllingPlayer(NULL);
  team->setSupportingPlayer(NULL);
  team->setReceiver(NULL);
  team->setPlayerClosestToBall(NULL);

  // 向各球员发送回到本方区域的消息。
  team->returnAllEntityPlayerOnFieldsToHome();
}

void PrepareForKickOff::execute()
{
  auto* team = &mOwner;
  // 双方球员全部回到区域后开始比赛。
  if (team->allPlayersAtHome() && team->opponents()->allPlayersAtHome())
  {
    team->getAi()->changeState(TeamState::defending);
  }
}

void PrepareForKickOff::exit()
{
  auto* team = &mOwner;
  team->pitch()->setGameOn();
}



Attacking::Attacking(SoccerTeam& owner) : mOwner(owner) {}

const char* Attacking::name() const { return "Attacking"; }

bool Attacking::onMessage(const Telegram& telegram) {return false;}


Defending::Defending(SoccerTeam& owner) : mOwner(owner) {}

const char* Defending::name() const { return "Defending"; }

bool Defending::onMessage(const Telegram& telegram) {return false;}


PrepareForKickOff::PrepareForKickOff(SoccerTeam& owner) : mOwner(owner) {}

const char* PrepareForKickOff::name() const { return "PrepareForKickOff"; }

bool PrepareForKickOff::onMessage(const Telegram& telegram) {return false;}

