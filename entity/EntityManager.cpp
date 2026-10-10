#include "EntityManager.h"
#include "EntityBase.h"


//--------------------------- instance ----------------------------------------
//
//   this class is a singleton
//-----------------------------------------------------------------------------
EntityManager* EntityManager::instance()
{
  static EntityManager instance;

  return &instance;
}

//------------------------- getEntityFromId -----------------------------------
//-----------------------------------------------------------------------------
EntityBase* EntityManager::getEntityFromId(int id)const
{
  //find the entity
  EntityMap::const_iterator ent = mEntityMap.find(id);

  //assert that the entity is a member of the map
  assert ( (ent !=  mEntityMap.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

  return ent->second;
}

//--------------------------- removeEntity ------------------------------------
//-----------------------------------------------------------------------------
void EntityManager::removeEntity(EntityBase* pEntity)
{
  mEntityMap.erase(mEntityMap.find(pEntity->id()));
}

//---------------------------- registerEntity ---------------------------------
//-----------------------------------------------------------------------------
void EntityManager::registerEntity(EntityBase* newEntity)
{
  mEntityMap.insert(std::make_pair(newEntity->id(), newEntity));
}
