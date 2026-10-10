/*
 * 阅读提示：同类对象自动登记模板，构造时加入列表，析构时移除。
 * 列表借用对象指针，不负责删除对象；模板为不同类型各自建立列表。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef AUTOLIST_H
#define AUTOLIST_H

// 自动登记模板：派生对象创建时加入同类对象列表，销毁时移除。原作者：Mat Buckland（fup@ai-junkie.com）。
#include <list>


template <class T>
class AutoList
{
public:

  typedef std::list<T*> ObjectList;

private:

  static ObjectList mMembers;

protected:

  AutoList()
  {
    // 将当前对象转换为派生类指针并登记；这里使用模板把列表限定为同一类对象。
    mMembers.push_back(static_cast<T*>(this));
  }

  ~AutoList()
  {
    mMembers.remove(static_cast<T*>(this));
  }

public:


  static ObjectList& getAllMembers(){return mMembers;}
};


template <class T>
std::list<T*> AutoList<T>::mMembers;



#endif