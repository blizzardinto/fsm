/*
 * 阅读提示：实体查找服务，把唯一编号映射到对象指针，供消息系统定位接收者。
 * 登记一个指针不等于拥有对象；这里的移除和清空仅操作记录，对象仍由原来的拥有者销毁。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "EntityManager.h"
#include "EntityBase.h"


// 单例入口：返回共享的实体查找服务。
EntityManager* EntityManager::instance()
{
  static EntityManager instance;

  return &instance;
}

// 根据编号查询实体对象。
EntityBase* EntityManager::getEntityFromId(int id)const
{
  // 在编号到对象指针的映射中查找。
  EntityMap::const_iterator ent = mEntityMap.find(id);

  // 断言该编号已经注册；这里不是创建新对象。
  assert ( (ent !=  mEntityMap.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

  return ent->second;
}

// 移除实体的注册记录，不负责销毁实体。
void EntityManager::removeEntity(EntityBase* pEntity)
{
  mEntityMap.erase(mEntityMap.find(pEntity->id()));
}

// 把实体指针登记到编号映射中。
void EntityManager::registerEntity(EntityBase* newEntity)
{
  mEntityMap.insert(std::make_pair(newEntity->id(), newEntity));
}
