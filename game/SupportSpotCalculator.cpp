#include "SupportSpotCalculator.h"
#include "EntityPlayer.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "constants.h"
#include "regulator.h"
#include "SoccerTeam.h"
#include "ParamLoader.h"
#include "SoccerPitch.h"

#include "DebugConsole.h"

//------------------------------- dtor ----------------------------------------
//-----------------------------------------------------------------------------
SupportSpotCalculator::~SupportSpotCalculator()
{
  delete mRegulator;
}


//------------------------------- ctor ----------------------------------------
//-----------------------------------------------------------------------------
SupportSpotCalculator::SupportSpotCalculator(int           numX,
                                             int           numY,
                                             SoccerTeam*   team):mBestSupportingSpot(NULL),
                                                                  mTeam(team)
{
  const Region* playingField = team->pitch()->playingArea();

  //calculate the positions of each sweet spot, create them and
  //store them in mSpots
  double heightOfSupportSpotRegion = playingField->height() * 0.8;
  double widthOfSupportSpotRegion  = playingField->width() * 0.9;
  double sliceX = widthOfSupportSpotRegion / numX ;
  double sliceY = heightOfSupportSpotRegion / numY;

  double left  = playingField->left() + (playingField->width()-widthOfSupportSpotRegion)/2.0 + sliceX/2.0;
  double right = playingField->right() - (playingField->width()-widthOfSupportSpotRegion)/2.0 - sliceX/2.0;
  double top   = playingField->top() + (playingField->height()-heightOfSupportSpotRegion)/2.0 + sliceY/2.0;

  for (int x=0; x<(numX/2)-1; ++x)
  {
    for (int y=0; y<numY; ++y)
    {
      if (mTeam->color() == SoccerTeam::blue)
      {
        mSpots.push_back(SupportSpot(Vector2D(left+x*sliceX, top+y*sliceY), 0.0));
      }

      else
      {
        mSpots.push_back(SupportSpot(Vector2D(right-x*sliceX, top+y*sliceY), 0.0));
      }
    }
  }

  //create the regulator
  mRegulator = new Regulator(prm.supportSpotUpdateFreq);
}


//--------------------------- determineBestSupportingPosition -----------------
//
//  see header or book for description
//-----------------------------------------------------------------------------
Vector2D SupportSpotCalculator::determineBestSupportingPosition()
{
  //only update the spots every few frames
  if (!mRegulator->isReady() && mBestSupportingSpot)
  {
    return mBestSupportingSpot->mPos;
  }

  //reset the best supporting spot
  mBestSupportingSpot = NULL;

  double bestScoreSoFar = 0.0;

  std::vector<SupportSpot>::iterator curSpot;

  for (curSpot = mSpots.begin(); curSpot != mSpots.end(); ++curSpot)
  {
    //first remove any previous score. (the score is set to one so that
    //the viewer can see the positions of all the spots if he has the
    //aids turned on)
    curSpot->mScore = 1.0;

    //Test 1. is it possible to make a safe pass from the ball's position
    //to this position?
    if(mTeam->isPassSafeFromAllOpponents(mTeam->controllingPlayer()->pos(),
                                           curSpot->mPos,
                                           NULL,
                                           prm.maxPassingForce))
    {
      curSpot->mScore += prm.spotPassSafeScore;
    }


    //Test 2. Determine if a goal can be scored from this position.
    if( mTeam->canShoot(curSpot->mPos,
                          prm.maxShootingForce))
    {
      curSpot->mScore += prm.spotCanScoreFromPositionScore;
    }


    //Test 3. calculate how far this spot is away from the controlling
    //player. The further away, the higher the score. Any distances further
    //away than optimalDistance pixels do not receive a score.
    if (mTeam->supportingPlayer())
    {
      const double optimalDistance = 200.0;

      double dist = vec2DDistance(mTeam->controllingPlayer()->pos(),
                                 curSpot->mPos);

      double temp = fabs(optimalDistance - dist);

      if (temp < optimalDistance)
      {

        //normalize the distance and add it to the score
        curSpot->mScore += prm.spotDistFromControllingPlayerScore *
                             (optimalDistance-temp)/optimalDistance;
      }
    }

    //check to see if this spot has the highest score so far
    if (curSpot->mScore > bestScoreSoFar)
    {
      bestScoreSoFar = curSpot->mScore;

      mBestSupportingSpot = &(*curSpot);
    }

  }

  return mBestSupportingSpot->mPos;
}





//------------------------------- getBestSupportingSpot -----------------------
//-----------------------------------------------------------------------------
Vector2D SupportSpotCalculator::getBestSupportingSpot()
{
  if (mBestSupportingSpot)
  {
    return mBestSupportingSpot->mPos;
  }

  else
  {
    return determineBestSupportingPosition();
  }
}

//----------------------------------- render ----------------------------------
//-----------------------------------------------------------------------------
void SupportSpotCalculator::render()const
{
    gdi->hollowBrush();
    gdi->greyPen();

    for (unsigned int spt=0; spt<mSpots.size(); ++spt)
    {
      gdi->circle(mSpots[spt].mPos, mSpots[spt].mScore);
    }

    if (mBestSupportingSpot)
    {
      gdi->greenPen();
      gdi->circle(mBestSupportingSpot->mPos, mBestSupportingSpot->mScore);
    }
}