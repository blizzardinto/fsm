/*
 * 阅读提示：场上球员具体类，继承球员公共能力，并拥有 FieldPlayerAI 控制器。
 * 继承用于表达它是一个球员；组合用于表达它有一个决策控制器，二者承担不同职责。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#pragma warning (disable:4786)
#ifndef FIELDPLAYER_H
#define FIELDPLAYER_H
// 场上球员派生类：在公共球员能力上增加踢球、带球、接球等 AI 决策。原作者：Mat Buckland，2003（fup@ai-junkie.com）。
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

#include "Vector2D.h"
#include "EntityPlayer.h"
#include "FieldPlayerAI.h"
#include <memory>

class Regulator;
class SoccerTeam;
class SoccerPitch;
class Goal;
struct Telegram;


class EntityPlayerOnField : public EntityPlayer
{
private:

   // 智能指针独占 AI 控制器；球员销毁时控制器及其状态自动销毁。
  std::unique_ptr<FieldPlayerAI> mAi;

  // 调节器限制每秒允许的踢球次数。
  Regulator*                  mKickLimiter;


public:

  EntityPlayerOnField(SoccerTeam*    homeTeam,
             int        homeRegion,
             FieldPlayerState startState,
             Vector2D  heading,
             Vector2D      velocity,
             double         mass,
             double         maxForce,
             double         maxSpeed,
             double         maxTurnRate,
             double         scale,
             PlayerRole    role);

  ~EntityPlayerOnField();

  // 更新球员的位置和朝向。
  void        update();

  void        render();

  bool        handleMessage(const Telegram& msg);

  FieldPlayerAI* getAi()const;

  bool        isReadyForNextKick()const;


};




#endif
