#include "EntityBase.h"


int EntityBase::mNextValidId = 0;

//------------------------------ ctor -----------------------------------------
//-----------------------------------------------------------------------------
EntityBase::EntityBase(int id):mBoundingRadius(0.0),
                                       mScale(Vector2D(1.0,1.0)),
                                       mType(defaultEntityType),
                                       mTag(false)
{
  setId(id);
}

//----------------------------- setId -----------------------------------------
//
//  this must be called within each function Object() { [native code] } to make sure the id is set
//  correctly. It verifies that the value passed to the method is greater
//  or equal to the next valid id, before setting the id and incrementing
//  the next valid id
//-----------------------------------------------------------------------------
void EntityBase::setId(int val)
{
  //make sure the val is equal to or greater than the next available id
  assert ( (val >= mNextValidId) && "<EntityBase::SetID>: invalid ID");

  mId = val;

  mNextValidId = mId + 1;
}
