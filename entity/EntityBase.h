/*
 * 阅读提示：实体抽象基类：封装身份编号、位置、大小和公共更新接口。
 * 把球员和足球当作实体使用时，调用者依赖基类接口；派生类通过虚函数提供具体实现。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
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

// 私有成员只由本类方法管理；外部通过接口访问，避免随意破坏编号等约束。
private:

  // 每个实体都有唯一编号，消息系统据此查找对象。
  int         mId;

  // 实体的类型编号，可用于区分不同类别。
  int         mType;

  // 通用标记，邻居筛选等算法可以临时使用它。
  bool        mTag;

  // 类的静态成员保存下一个可用编号，由所有实体共享。
  static int  mNextValidId;

  // 设置编号并更新静态计数器，保证后续对象取得不同编号。
  void setId(int val);


// 受保护成员允许派生类使用，但普通调用者仍不能直接访问。
protected:

  // 实体在二维世界中的位置。
  Vector2D mPosition;

  Vector2D mScale;

  // 包围圆半径，用于简化碰撞和距离判断。
  double    mBoundingRadius;


  EntityBase(int id);

public:

  // 虚析构：通过基类指针删除球员或足球时，也会先执行具体派生类的析构。
  virtual ~EntityBase(){}

  virtual void update(){};

  // 纯虚函数使本类成为抽象类，不能直接创建 EntityBase；具体实体必须实现绘制。
  virtual void render()=0;

  virtual bool handleMessage(const Telegram& msg){return false;}

  // 可由派生类提供读写流的实现。
  virtual void write(std::ostream&  os)const{}
  virtual void read (std::ifstream& is){}

  // 获取下一个可用实体编号。
  static int   getNextValidId(){return mNextValidId;}

  // 重置编号计数器；调用方需要保证现有实体不会发生编号冲突。
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




