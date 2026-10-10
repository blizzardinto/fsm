#ifndef GAME_ENTITY_FUNCTION_TEMPLATES
#define GAME_ENTITY_FUNCTION_TEMPLATES

#include "EntityBase.h"
#include "geometry.h"



//////////////////////////////////////////////////////////////////////////
//
//  Some useful template functions
//
//////////////////////////////////////////////////////////////////////////

//------------------------- overlapped -----------------------------------
//
//  tests to see if an entity is overlapping any of a number of entities
//  stored in a std container
//------------------------------------------------------------------------
template <class T, class conT>
bool overlapped(const T* ob, const conT& conOb, double minDistBetweenObstacles = 40.0)
{
  typename conT::const_iterator it;

  for (it=conOb.begin(); it != conOb.end(); ++it)
  {
    if (twoCirclesOverlapped(ob->pos(),
                             ob->boundingRadius()+minDistBetweenObstacles,
                             (*it)->pos(),
                             (*it)->boundingRadius()))
    {
      return true;
    }
  }

  return false;
}

//----------------------- tagNeighbors ----------------------------------
//
//  tags any entities contained in a std container that are within the
//  radius of the single entity parameter
//------------------------------------------------------------------------
template <class T, class conT>
void tagNeighbors(T* entity, conT& others, const double radius)
{
  typename conT::iterator it;

  //iterate through all entities checking for range
  for (it=others.begin(); it != others.end(); ++it)
  {
    //first clear any current tag
    (*it)->unTag();

    //work in distance squared to avoid sqrts
    Vector2D to = (*it)->pos() - entity->pos();

    //the bounding radius of the other is taken into account by adding it
    //to the range
    double range = radius + (*it)->boundingRadius();

    //if entity within range, tag for further consideration
    if ( ((*it) != entity) && (to.lengthSq() < range*range))
    {
      (*it)->tag();
    }

  }//next entity
}


//------------------- enforceNonPenetrationContraint ---------------------
//
//  Given a pointer to an entity and a std container of pointers to nearby
//  entities, this function checks to see if there is an overlap between
//  entities. If there is, then the entities are moved away from each
//  other
//------------------------------------------------------------------------
template <class T, class conT>
void enforceNonPenetrationContraint(T entity, const conT& others)
{
  typename conT::const_iterator it;

  //iterate through all entities checking for any overlap of bounding
  //radii
  for (it=others.begin(); it != others.end(); ++it)
  {
    //make sure we don't check against this entity
    if (*it == entity) continue;

    //calculate the distance between the positions of the entities
    Vector2D toEntity = entity->pos() - (*it)->pos();

    double distFromEachOther = toEntity.length();

    //if this distance is smaller than the sum of their radii then this
    //entity must be moved away in the direction parallel to the
    //toEntity vector
    double amountOfOverLap = (*it)->boundingRadius() + entity->boundingRadius() -
                             distFromEachOther;

    if (amountOfOverLap >= 0)
    {
      //move the entity a distance away equivalent to the amount of overlap.
      entity->setPos(entity->pos() + (toEntity/distFromEachOther) *
                     amountOfOverLap);
    }
  }//next entity
}










#endif