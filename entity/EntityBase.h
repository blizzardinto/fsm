 #ifndef BASE_GAME_ENTITY_H
#define BASE_GAME_ENTITY_H
#pragma warning (disable:4786)
#include <vector>
#include <string>
#include <iosfwd>
#include "Vector2D.h"
#include "Geometry.h"
#include "utils.h"



struct Telegram;


class EntityBase
{
public:

  enum {defaultEntityType = -1};

private:

  //each entity has a unique id
  int         mId;

  //every entity has a type associated with it (health, troll, ammo etc)
  int         mType;

  //this is a generic flag.
  bool        mTag;

  //this is the next valid id. Each time a EntityBase is instantiated
  //this value is updated
  static int  mNextValidId;

  //this must be called within each function Object() { [native code] } to make sure the id is set
  //correctly. It verifies that the value passed to the method is greater
  //or equal to the next valid id, before setting the id and incrementing
  //the next valid id
  void setId(int val);


protected:

  //its location in the environment
  Vector2D mPosition;

  Vector2D mScale;

  //the magnitude of this object's bounding radius
  double    mBoundingRadius;


  EntityBase(int id);

public:

  virtual ~EntityBase(){}

  virtual void update(){};

  virtual void render()=0;

  virtual bool handleMessage(const Telegram& msg){return false;}

  //entities should be able to read/write their data to a stream
  virtual void write(std::ostream&  os)const{}
  virtual void read (std::ifstream& is){}

  //use this to grab the next valid id
  static int   getNextValidId(){return mNextValidId;}

  //this can be used to reset the next id
  static void  resetNextValidId(){mNextValidId = 0;}



  Vector2D     pos()const{return mPosition;}
  void         setPos(Vector2D newPos){mPosition = newPos;}

  double       boundingRadius()const{return mBoundingRadius;}
  void         setBoundingRadius(double r){mBoundingRadius = r;}
  int          id()const{return mId;}

  bool         isTagged()const{return mTag;}
  void         tag(){mTag = true;}
  void         unTag(){mTag = false;}

  Vector2D     scale()const{return mScale;}
  void         setScale(Vector2D val){mBoundingRadius *= maxOf(val.x, val.y)/maxOf(mScale.x, mScale.y); mScale = val;}
  void         setScale(double val){mBoundingRadius *= (val/maxOf(mScale.x, mScale.y)); mScale = Vector2D(val, val);}

  int          entityType()const{return mType;}
  void         setEntityType(int newType){mType = newType;}

};




#endif




