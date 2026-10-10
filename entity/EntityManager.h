/*
 * 阅读提示：实体查找服务，把唯一编号映射到对象指针，供消息系统定位接收者。
 * 登记一个指针不等于拥有对象；这里的移除和清空仅操作记录，对象仍由原来的拥有者销毁。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#pragma warning (disable:4786)
// 实体管理器维护编号到实体指针的映射。原作者：Mat Buckland（fup@ai-junkie.com）。
#include <map>
#include <cassert>


class EntityBase;

// 提供访问实体管理器的简写。
#define entityMgr EntityManager::instance()



class EntityManager
{
private:

  typedef std::map<int, EntityBase*> EntityMap;

private:

  // 用映射按编号快速查找对象；保存的是借用指针，不代表拥有对象。
  EntityMap mEntityMap;

  EntityManager(){}

  // 禁止复制和赋值，保证只有一个共享的管理器。
  EntityManager(const EntityManager&);
  EntityManager& operator=(const EntityManager&);

public:

  static EntityManager* instance();

  // 注册对象指针，并用实体编号作为映射键。
  void            registerEntity(EntityBase* newEntity);

  // 根据编号返回实体指针。
  EntityBase* getEntityFromId(int id)const;

  // 移除对象的登记记录。
  void            removeEntity(EntityBase* pEntity);

  // 清空查找映射；不会删除映射指向的实体。
  void            reset(){mEntityMap.clear();}
};







#endif