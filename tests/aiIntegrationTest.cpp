/*
 * 阅读提示：AI 集成测试，创建真实比赛对象验证状态隔离、消息处理、开球及连续模拟。
 * 单元测试验证局部规则，集成测试验证多个对象能否按约定一起工作。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "EntityManager.h"
#include "EntityPlayer.h"
#include "EntityPlayerGoalKeeper.h"
#include "EntityPlayerOnField.h"
#include "SoccerBall.h"
#include "SoccerMessages.h"
#include "SoccerPitch.h"
#include "SoccerTeam.h"
#include "SteeringBehaviors.h"
#include "Telegram.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <vector>

static_assert(!std::is_invocable_v<decltype(&FieldPlayerAI::changeState),
                                 FieldPlayerAI&, TeamState>);
static_assert(!std::is_invocable_v<decltype(&GoalkeeperAI::changeState),
                                 GoalkeeperAI&, FieldPlayerState>);
static_assert(!std::is_copy_constructible_v<FieldPlayerAI>);

namespace
{
void require(bool condition, const char* message)
{
  if (!condition) throw std::runtime_error(message);
}

void runMatch()
{
  SoccerPitch pitch(1400, 800);
  std::vector<EntityPlayerOnField*> players;
  std::vector<EntityPlayerGoalKeeper*> keepers;
  std::vector<SoccerTeam*> teams;
  for (auto* player : AutoList<EntityPlayer>::getAllMembers())
  {
    if (player->role() == EntityPlayer::goalKeeper)
    {
      auto* keeper = static_cast<EntityPlayerGoalKeeper*>(player);
      keepers.push_back(keeper);
      teams.push_back(player->team());
      require(keeper->getAi()->isInState(GoalkeeperState::tendGoal), "Keeper initial state");
    }
    else
    {
      auto* fieldPlayer = static_cast<EntityPlayerOnField*>(player);
      players.push_back(fieldPlayer);
      require(fieldPlayer->getAi()->isInState(FieldPlayerState::wait), "Player initial state");
    }
  }
  require(players.size() == 8 && keepers.size() == 2, "Match entity count");
  require(teams[0]->getAi()->isInState(TeamState::defending), "Team initial state");

  auto* first = players[0];
  auto* second = players[1];
  Vector2D target(300, 250);
  Telegram receive(0, 0, first->id(), msgReceiveBall, &target);
  require(first->handleMessage(receive), "Receive message not handled");
  require(first->getAi()->isInState(FieldPlayerState::receiveBall), "Receive transition");
  require(second->getAi()->isInState(FieldPlayerState::wait), "States leaked between players");
  require(first->steering()->target() == target, "Receive target bound to wrong owner");
  first->getAi()->changeState(FieldPlayerState::wait);

  keepers[0]->getAi()->changeState(GoalkeeperState::returnHome);
  require(keepers[1]->getAi()->isInState(GoalkeeperState::tendGoal), "Keeper states shared");
  keepers[0]->getAi()->changeState(GoalkeeperState::tendGoal);

  bool rejectedInvalidState = false;
  try { first->getAi()->changeState(static_cast<FieldPlayerState>(999)); }
  catch (const std::invalid_argument&) { rejectedInvalidState = true; }
  require(rejectedInvalidState && first->getAi()->isInState(FieldPlayerState::wait),
          "Invalid state damaged the machine");

  pitch.setGameOff();
  for (auto* team : teams) team->getAi()->changeState(TeamState::prepareForKickOff);
  for (int frame = 0; frame < 3000 && !pitch.gameOn(); ++frame) pitch.update();
  require(pitch.gameOn(), "Kickoff did not finish");

  for (int frame = 0; frame < 3000; ++frame)
  {
    pitch.update();
    require(std::isfinite(pitch.ball()->pos().x) && std::isfinite(pitch.ball()->pos().y),
            "Invalid ball position");
    for (auto* player : AutoList<EntityPlayer>::getAllMembers())
    {
      require(std::isfinite(player->pos().x) && std::isfinite(player->pos().y),
              "Invalid player position");
    }
  }
}
}

int main()
{
  try
  {
    for (int match = 0; match < 3; ++match)
    {
      std::srand(42 + match);
      runMatch();
      require(AutoList<EntityPlayer>::getAllMembers().empty(), "Players survived destruction");
      // 原有全局注册表尚未自动注销实体，因此测试结束后显式清空记录。
      entityMgr->reset();
    }
    std::cout << "AI integration tests passed (three matches, 9000 simulation updates)\n";
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
