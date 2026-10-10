/*
 * 阅读提示：实体集合的通用算法，提供邻居标记和重叠分离等操作。
 * 模板让不同容器或实体类型复用算法，但参数类型仍需具备算法调用的位置和半径接口。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef GAME_ENTITY_FUNCTION_TEMPLATES
#define GAME_ENTITY_FUNCTION_TEMPLATES

#include "EntityBase.h"
#include "geometry.h"



// 实体辅助模板：检查一个实体是否与容器中的其他实体发生包围圆重叠。
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

// 标记指定半径范围内的邻居实体。
template <class T, class conT>
void tagNeighbors(T* entity, conT& others, const double radius)
{
  typename conT::iterator it;

  // 遍历容器中的实体并检查距离。
  for (it=others.begin(); it != others.end(); ++it)
  {
    // 清除上一轮留下的标记。
    (*it)->unTag();

    // 比较距离平方，避免不必要的开平方运算。
    Vector2D to = (*it)->pos() - entity->pos();

    // 判断范围时考虑另一个实体的包围圆半径。
    double range = radius + (*it)->boundingRadius();

    // 范围内的实体被标记，供后续算法使用。
    if ( ((*it) != entity) && (to.lengthSq() < range*range))
    {
      (*it)->tag();
    }

  }// 处理下一个实体。
}


// 分离重叠实体：检查包围圆重叠，把当前实体沿远离邻居的方向推开。
template <class T, class conT>
void enforceNonPenetrationContraint(T entity, const conT& others)
{
  typename conT::const_iterator it;

  // 遍历其他实体，检查包围圆是否重叠。
  for (it=others.begin(); it != others.end(); ++it)
  {
    // 跳过当前实体自身。
    if (*it == entity) continue;

    // 计算两个实体的位置差向量。
    Vector2D toEntity = entity->pos() - (*it)->pos();

    double distFromEachOther = toEntity.length();

    // 距离小于半径之和时发生重叠，需要沿位置差方向移动当前实体。
    double amountOfOverLap = (*it)->boundingRadius() + entity->boundingRadius() -
                             distFromEachOther;

    if (amountOfOverLap >= 0)
    {
      // 移动距离等于包围圆的重叠量。
      entity->setPos(entity->pos() + (toEntity/distFromEachOther) *
                     amountOfOverLap);
    }
  }// 处理下一个实体。
}










#endif