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

  ParamLoader():IniFileLoaderBase("Params.ini")
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

  //these values tweak the various rules used to calculate the support spots
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

  //max steering force
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

  //in the range zero to 1.0. adjusts the amount of noise added to a kick,
  //the lower the value the worse the players get
  double  playerKickingAccuracy;

  //the number of times the SoccerTeam::canShoot method attempts to find
  //a valid shot
  int    numAttemptsToFindValidStrike;

  //the distance away from the center of its home region a player
  //must be to be considered at home
  double withinRangeOfHome;

  //how close a player must get to a sweet spot before he can change state
  double withinRangeOfSupportSpot;
  double withinRangeOfSupportSpotSq;


  //the minimum distance a receiving player must be from the passing player
  double   minPassDist;
  double   goalkeeperMinPassDist;

  //this is the distance the keeper puts between the back of the net
  //and the ball when using the interposeBehavior steering behavior
  double  entityPlayerGoalKeeperTendingDistance;

  //when the ball becomes within this distance of the goalkeeper he
  //changes state to intercept the ball
  double  entityPlayerGoalKeeperInterceptRange;
  double  entityPlayerGoalKeeperInterceptRangeSq;

  //how close the ball must be to a receiver before he starts chasing it
  double  ballWithinReceivingRange;
  double  ballWithinReceivingRangeSq;


  //these values control what debug info you can see
  bool  bStates;
  bool  bIds;
  bool  bSupportSpots;
  bool  bRegions;
  bool  bShowControllingTeam;
  bool  bViewTargets;
  bool  bHighlightIfThreatened;

  int frameRate;


  double separationCoefficient;

  //how close a neighbour must be before an agent perceives it
  double viewDistance;

  //zero this to turn the constraint off
  bool bNonPenetrationConstraint;

};

#endif