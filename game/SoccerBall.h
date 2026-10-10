#ifndef SOCCERBALL_H
#define SOCCERBALL_H
#pragma warning (disable:4786)
#include <vector>

#include "EntityMovable.h"
#include "constants.h"


class Wall2D;
class EntityPlayer;


class SoccerBall : public EntityMovable
{
private:

  //keeps a record of the ball's position at the last update
  Vector2D                  mOldPos;

  //a local reference to the walls that make up the pitch boundary
  const std::vector<Wall2D>& mPitchBoundary;




public:
    //tests to see if the ball has collided with a ball and reflects
  //the ball's velocity accordingly
  void testCollisionWithWalls(const std::vector<Wall2D>& walls);

  SoccerBall(Vector2D           pos,
             double               ballSize,
             double               mass,
             std::vector<Wall2D>& pitchBoundary):

      //set up the base class
      EntityMovable(pos,
                  ballSize,
                  Vector2D(0,0),
                  -1.0,                //max speed - unused
                  Vector2D(0,1),
                  mass,
                  Vector2D(1.0,1.0),  //scale     - unused
                  0,                   //turn rate - unused
                  0),                  //max force - unused
     mPitchBoundary(pitchBoundary)
  {}

  //implement base class update
  void      update();

  //implement base class render
  void      render();

  //a soccer ball doesn't need to handle messages
  bool      handleMessage(const Telegram& msg){return false;}

  //this method applies a directional force to the ball (kicks it!)
  void      kick(Vector2D direction, double force);

  //given a kicking force and a distance to traverse defined by start
  //and finish points, this method calculates how long it will take the
  //ball to cover the distance.
  double    timeToCoverDistance(Vector2D from,
                               Vector2D to,
                               double     force)const;

  //this method calculates where the ball will in 'time' seconds
  Vector2D futurePosition(double time)const;

  //this is used by players and goalkeepers to 'trap' a ball -- to stop
  //it dead. That player is then assumed to be in possession of the ball
  //and mOwner is adjusted accordingly
  void      trap(){mVelocity.zero();}

  Vector2D  oldPos()const{return mOldPos;}

  //this places the ball at the desired location and sets its velocity to zero
  void      placeAtPosition(Vector2D newPos);
};



//this can be used to vary the accuracy of a player's kick.
Vector2D addNoiseToKick(Vector2D ballPos, Vector2D ballTarget);



#endif