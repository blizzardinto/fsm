#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "Goal.h"
#include "Region.h"
#include "Transformations.h"
#include "Geometry.h"
#include "SoccerTeam.h"
#include "DebugConsole.h"
#include "EntityManager.h"
#include "ParamLoader.h"
#include "EntityPlayer.h"
#include "StatesTeam.h"
#include "FrameCounter.h"

const int numRegionsHorizontal = 6;
const int numRegionsVertical   = 3;

//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
SoccerPitch::SoccerPitch(int cx, int cy):mClientWidth(cx),
                                         mClientHeight(cy),
                                         mPaused(false),
                                         mEntityPlayerGoalKeeperHasBall(false),
                                         mRegions(numRegionsHorizontal*numRegionsVertical),
                                         mGameOn(true)
{
  //define the playing area
  mPlayingArea = new Region(20, 20, cx-20, cy-20);

  //create the regions
  createRegions(playingArea()->width() / (double)numRegionsHorizontal,
                playingArea()->height() / (double)numRegionsVertical);

  //create the goals
   mRedGoal  = new Goal(Vector2D( mPlayingArea->left(), (cy-prm.goalWidth)/2),
                          Vector2D(mPlayingArea->left(), cy - (cy-prm.goalWidth)/2),
                          Vector2D(1,0));



  mBlueGoal = new Goal( Vector2D( mPlayingArea->right(), (cy-prm.goalWidth)/2),
                          Vector2D(mPlayingArea->right(), cy - (cy-prm.goalWidth)/2),
                          Vector2D(-1,0));


  //create the soccer ball
  mBall = new SoccerBall(Vector2D((double)mClientWidth/2.0, (double)mClientHeight/2.0),
                           prm.ballSize,
                           prm.ballMass,
                           mWalls);


  //create the teams
  mRedTeam  = new SoccerTeam(mRedGoal, mBlueGoal, this, SoccerTeam::red);
  mBlueTeam = new SoccerTeam(mBlueGoal, mRedGoal, this, SoccerTeam::blue);

  //make sure each team knows who their opponents are
  mRedTeam->setOpponents(mBlueTeam);
  mBlueTeam->setOpponents(mRedTeam);

  //create the walls
  Vector2D topLeft(mPlayingArea->left(), mPlayingArea->top());
  Vector2D topRight(mPlayingArea->right(), mPlayingArea->top());
  Vector2D bottomRight(mPlayingArea->right(), mPlayingArea->bottom());
  Vector2D bottomLeft(mPlayingArea->left(), mPlayingArea->bottom());

  mWalls.push_back(Wall2D(bottomLeft, mRedGoal->rightPost()));
  mWalls.push_back(Wall2D(mRedGoal->leftPost(), topLeft));
  mWalls.push_back(Wall2D(topLeft, topRight));
  mWalls.push_back(Wall2D(topRight, mBlueGoal->leftPost()));
  mWalls.push_back(Wall2D(mBlueGoal->rightPost(), bottomRight));
  mWalls.push_back(Wall2D(bottomRight, bottomLeft));

  ParamLoader* p = ParamLoader::instance();
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
SoccerPitch::~SoccerPitch()
{
  delete mBall;

  delete mRedTeam;
  delete mBlueTeam;

  delete mRedGoal;
  delete mBlueGoal;

  delete mPlayingArea;

  for (unsigned int i=0; i<mRegions.size(); ++i)
  {
    delete mRegions[i];
  }
}

//----------------------------- update -----------------------------------
//
//  this demo works on a fixed frame rate (60 by default) so we don't need
//  to pass a time_elapsed as a parameter to the game entities
//------------------------------------------------------------------------
void SoccerPitch::update()
{
  if (mPaused) return;

  static int tick = 0;

  //update the balls
  mBall->update();

  //update the teams
  mRedTeam->update();
  mBlueTeam->update();

  //if a goal has been detected reset the pitch ready for kickoff
  if (mBlueGoal->scored(mBall) || mRedGoal->scored(mBall))
  {
    mGameOn = false;

    //reset the ball
    mBall->placeAtPosition(Vector2D((double)mClientWidth/2.0, (double)mClientHeight/2.0));

    //get the teams ready for kickoff
    mRedTeam->getFsm()->changeState(PrepareForKickOff::instance());
    mBlueTeam->getFsm()->changeState(PrepareForKickOff::instance());
  }
}

//------------------------- createRegions --------------------------------
void SoccerPitch::createRegions(double width, double height)
{
  //index into the vector
  int idx = mRegions.size()-1;

  for (int col=0; col<numRegionsHorizontal; ++col)
  {
    for (int row=0; row<numRegionsVertical; ++row)
    {
      mRegions[idx--] = new Region(playingArea()->left()+col*width,
                                   playingArea()->top()+row*height,
                                   playingArea()->left()+(col+1)*width,
                                   playingArea()->top()+(row+1)*height,
                                   idx);
    }
  }
}


//------------------------------ render ----------------------------------
//------------------------------------------------------------------------
bool SoccerPitch::render()
{
  //draw the grass
  gdi->darkGreenPen();
  gdi->darkGreenBrush();
  gdi->rect(0,0,mClientWidth, mClientHeight);

  //render regions
  if (prm.bRegions)
  {
    for (unsigned int r=0; r<mRegions.size(); ++r)
    {
      mRegions[r]->render(true);
    }
  }

  //render the goals
  gdi->hollowBrush();
  gdi->redPen();
  gdi->rect(mPlayingArea->left(), (mClientHeight-prm.goalWidth)/2, mPlayingArea->left()+40, mClientHeight - (mClientHeight-prm.goalWidth)/2);

  gdi->bluePen();
  gdi->rect(mPlayingArea->right(), (mClientHeight-prm.goalWidth)/2, mPlayingArea->right()-40, mClientHeight - (mClientHeight-prm.goalWidth)/2);

  //render the pitch markings
  gdi->whitePen();
  gdi->circle(mPlayingArea->center(), mPlayingArea->width() * 0.125);
  gdi->line(mPlayingArea->center().x, mPlayingArea->top(), mPlayingArea->center().x, mPlayingArea->bottom());
  gdi->whiteBrush();
  gdi->circle(mPlayingArea->center(), 2.0);


  //the ball
  gdi->whitePen();
  gdi->whiteBrush();
  mBall->render();

  //render the teams
  mRedTeam->render();
  mBlueTeam->render();

  //render the walls
  gdi->whitePen();
  for (unsigned int w=0; w<mWalls.size(); ++w)
  {
    mWalls[w].render();
  }

  //show the score
  gdi->textColor(Cgdi::red);
  gdi->textAtPos((mClientWidth/2)-50, mClientHeight-18, "Red: " + ttos(mBlueGoal->numGoalsScored()));

  gdi->textColor(Cgdi::blue);
  gdi->textAtPos((mClientWidth/2)+10, mClientHeight-18, "Blue: " + ttos(mRedGoal->numGoalsScored()));

  return true;
}







