#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  name:   EntityManager.h
//
//  Desc:   Singleton class to handle the  management of Entities.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <map>
#include <cassert>


class EntityBase;

//provide easy access
#define entityMgr EntityManager::instance()



class EntityManager
{
private:

  typedef std::map<int, EntityBase*> EntityMap;

private:

  //to facilitate quick lookup the entities are stored in a std::map, in which
  //pointers to entities are cross referenced by their identifying number
  EntityMap mEntityMap;

  EntityManager(){}

  //copy ctor and assignment should be private
  EntityManager(const EntityManager&);
  EntityManager& operator=(const EntityManager&);

public:

  static EntityManager* instance();

  //this method stores a pointer to the entity in the std::vector
  //m_Entities at the index position indicated by the entity's id
  //(makes for faster access)
  void            registerEntity(EntityBase* newEntity);

  //returns a pointer to the entity with the id given as a parameter
  EntityBase* getEntityFromId(int id)const;

  //this method removes the entity from the list
  void            removeEntity(EntityBase* pEntity);

  //clears all entities from the entity map
  void            reset(){mEntityMap.clear();}
};







#endif