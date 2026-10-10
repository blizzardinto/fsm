/*
 * 阅读提示：实体抽象基类：封装身份编号、位置、大小和公共更新接口。
 * 把球员和足球当作实体使用时，调用者依赖基类接口；派生类通过虚函数提供具体实现。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "EntityBase.h"


int EntityBase::mNextValidId = 0;

// 构造实体时初始化身份、位置和包围半径。
EntityBase::EntityBase(int id):mBoundingRadius(0.0),
                                       mScale(Vector2D(1.0,1.0)),
                                       mType(defaultEntityType),
                                       mTag(false)
{
  setId(id);
}

// 设置唯一编号：要求编号不小于下一个可用编号，然后推进编号计数器。
void EntityBase::setId(int val)
{
  // 检查传入编号不会与已分配的编号冲突。
  assert ( (val >= mNextValidId) && "<EntityBase::SetID>: invalid ID");

  mId = val;

  mNextValidId = mId + 1;
}
