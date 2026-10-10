/*
 * 阅读提示：参数加载对象，复用配置读取基类，将按顺序读取的值保存为比赛参数。
 * 当前使用单例共享配置；文件顺序必须与读取顺序一致，参数名称本身不参与查找。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef PARAMLOADER
#define PARAMLOADER
#pragma warning(disable:4800)
#include <fstream>
#include <string>
#include <cassert>


#include "constants.h"
#include "iniFileLoaderBase.h"


#define prm (*ParamLoader::instance())

class ParamLoader : public IniFileLoaderBase
{
private:

  ParamLoader():IniFileLoaderBase("res/Params.ini")
  {

    goalWidth                   = getNextParameterDouble();

    numSupportSpotsX            = getNextParameterInt();
    numSupportSpotsY            = getNextParameterInt();

    spotPassSafeScore                     = getNextParameterDouble();
    spotCanScoreFromPositionScore         = getNextParameterDouble();
    spotDistFromControllingPlayerScore     = getNextParameterDouble();
    spotClosenessToSupportingPlayerScore  = getNextParameterDouble();
    spotAheadOfAttackerScore              = getNextParameterDouble();

    supportSpotUpdateFreq       = getNextParameterDouble();

    chancePlayerAttemptsPotShot = getNextParameterDouble();
    chanceOfUsingArriveTypeReceiveBehavior = getNextParameterDouble();

    ballSize                    = getNextParameterDouble();
    ballMass                    = getNextParameterDouble();
    friction                    = getNextParameterDouble();

    keeperInBallRange           = getNextParameterDouble();
    playerInTargetRange         = getNextParameterDouble();
    playerKickingDistance       = getNextParameterDouble();
    playerKickFrequency         = getNextParameterDouble();


    playerMass                  = getNextParameterDouble();
    playerMaxForce              = getNextParameterDouble();
    playerMaxSpeedWithBall      = getNextParameterDouble();
    playerMaxSpeedWithoutBall   = getNextParameterDouble();
    playerMaxTurnRate           = getNextParameterDouble();
    playerScale                 = getNextParameterDouble();
    playerComfortZone           = getNextParameterDouble();
    playerKickingAccuracy       = getNextParameterDouble();

    numAttemptsToFindValidStrike = getNextParameterInt();



    maxDribbleForce             = getNextParameterDouble();
    maxShootingForce            = getNextParameterDouble();
    maxPassingForce             = getNextParameterDouble();

    withinRangeOfHome           = getNextParameterDouble();
    withinRangeOfSupportSpot    = getNextParameterDouble();

    minPassDist                 = getNextParameterDouble();
    goalkeeperMinPassDist       = getNextParameterDouble();

    entityPlayerGoalKeeperTendingDistance   = getNextParameterDouble();
    entityPlayerGoalKeeperInterceptRange    = getNextParameterDouble();
    ballWithinReceivingRange    = getNextParameterDouble();

    bStates                     = getNextParameterBool();
    bIds                        = getNextParameterBool();
    bSupportSpots               = getNextParameterBool();
    bRegions                    = getNextParameterBool();
    bShowControllingTeam        = getNextParameterBool();
    bViewTargets                = getNextParameterBool();
    bHighlightIfThreatened      = getNextParameterBool();

    frameRate                   = getNextParameterInt();

    separationCoefficient       = getNextParameterDouble();
    viewDistance                = getNextParameterDouble();
    bNonPenetrationConstraint   = getNextParameterBool();


    ballWithinReceivingRangeSq = ballWithinReceivingRange * ballWithinReceivingRange;
    keeperInBallRangeSq      = keeperInBallRange * keeperInBallRange;
    playerInTargetRangeSq    = playerInTargetRange * playerInTargetRange;
    playerKickingDistance   += ballSize;
    playerKickingDistanceSq  = playerKickingDistance * playerKickingDistance;
    playerComfortZoneSq      = playerComfortZone * playerComfortZone;
    entityPlayerGoalKeeperInterceptRangeSq     = entityPlayerGoalKeeperInterceptRange * entityPlayerGoalKeeperInterceptRange;
    withinRangeOfSupportSpotSq = withinRangeOfSupportSpot * withinRangeOfSupportSpot;
  }

public:

  static ParamLoader* instance();

  double goalWidth;

  int   numSupportSpotsX;
  int   numSupportSpotsY;

  // 支援位置评分规则的权重和参数。
  double spotPassSafeScore;
  double spotCanScoreFromPositionScore;
  double spotDistFromControllingPlayerScore;
  double spotClosenessToSupportingPlayerScore;
  double spotAheadOfAttackerScore;

  double supportSpotUpdateFreq ;

  double chancePlayerAttemptsPotShot;
  double chanceOfUsingArriveTypeReceiveBehavior;

  double ballSize;
  double ballMass;
  double friction;

  double keeperInBallRange;
  double keeperInBallRangeSq;

  double playerInTargetRange;
  double playerInTargetRangeSq;

  double playerMass;

  // 最大移动驱动力。
  double playerMaxForce;
  double playerMaxSpeedWithBall;
  double playerMaxSpeedWithoutBall;
  double playerMaxTurnRate;
  double playerScale;
  double playerComfortZone;

  double playerKickingDistance;
  double playerKickingDistanceSq;

  double playerKickFrequency;

  double  maxDribbleForce;
  double  maxShootingForce;
  double  maxPassingForce;

  double  playerComfortZoneSq;

  // 踢球精度取零到一；值越小，加入的方向误差越大。
  double  playerKickingAccuracy;

  // 每次射门判断随机尝试的目标数量。
  int    numAttemptsToFindValidStrike;

  // 判断球员到家时采用的距离阈值。
  double withinRangeOfHome;

  // 判断球员是否到达目标位置的距离阈值。
  double withinRangeOfSupportSpot;
  double withinRangeOfSupportSpotSq;


  // 传球者与接球者之间允许的最小距离。
  double   minPassDist;
  double   goalkeeperMinPassDist;

  // 阻挡行为中守门员与球门侧目标之间的距离。
  double  entityPlayerGoalKeeperTendingDistance;

  // 足球接近到此距离时，守门员考虑切换到拦截状态。
  double  entityPlayerGoalKeeperInterceptRange;
  double  entityPlayerGoalKeeperInterceptRangeSq;

  // 接球队员距离足球足够近时，转入追球状态。
  double  ballWithinReceivingRange;
  double  ballWithinReceivingRangeSq;


  // 控制画面上显示哪些调试信息。
  bool  bStates;
  bool  bIds;
  bool  bSupportSpots;
  bool  bRegions;
  bool  bShowControllingTeam;
  bool  bViewTargets;
  bool  bHighlightIfThreatened;

  int frameRate;


  double separationCoefficient;

  // 邻居感知半径。
  double viewDistance;

  // 设为零时关闭重叠分离约束。
  bool bNonPenetrationConstraint;

};

#endif
