/*
 * 阅读提示：调试输出组件，封装窗口、缓冲区和日志文件。
 * 输出运算符让调用代码像写标准输出一样记录信息；禁用时使用相同接口的空接收器。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H
#pragma warning (disable:4786)
#include <vector>
#include <windows.h>
#include <iosfwd>
#include <fstream>

#include "utils.h"
#include "WindowUtils.h"


// 定义调试窗口使用的自定义消息。
const int umSetscroll = WM_USER + 32;

// 超过此行数时，把缓冲区内容写入日志文件。
const int maxBufferSize = 500;

// 调试窗口的初始尺寸。
const int debugWindowWidth  = 400;
const int debugWindowHeight = 400;

// 启用 DEBUG 宏才会使用调试控制台；否则消息交给空接收器。
#ifdef DEBUG
#define debugCon *(DebugConsole::instance())
#else
#define debugCon *(CSink::instance())
#endif

// 用这些开关控制调试输出的启用与禁用。
#define debugOn  DebugConsole::on();
#define debugOff DebugConsole::off();


// 空接收器提供相同的输出接口，但丢弃输入，便于关闭调试而不修改调用代码。
class CSink
{
private:

  CSink(){};

  // 禁止复制和赋值。
  CSink(const CSink&);
  CSink& operator=(const CSink&);

public:

  static CSink* instance(){static CSink instance; return &instance;}

  template<class T>
  CSink& operator<<(const T&)
  {
    return *this;
  }
};



class DebugConsole
{
private:

  static HWND	         mHwnd;

  // 字符串缓冲区保存待输出的调试信息。
  static std::vector<std::string> mBuffer;

  // 标记下一次输入是新建一行还是追加到当前行。
  static bool          mFlushed;

  // 调试窗口的位置。
  static int           mPosTop;
  static int           mPosLeft;

  // 标记窗口是否已销毁。
  static bool          mDestroyed;

  // 禁用时忽略输入。
  static bool          mActive;

  // 默认日志文件名。
  static std::ofstream mLogOut;



  // 调试窗口消息回调。
  static LRESULT CALLBACK debugWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  // 构造时注册窗口类并创建窗口。
  static bool             create();

  static void             drawWindow(){InvalidateRect(mHwnd, NULL, TRUE); UpdateWindow(mHwnd);}

private:

  DebugConsole(){}

  // 禁止复制和赋值，避免复制窗口和日志资源。
  DebugConsole(const DebugConsole&);
  DebugConsole& operator=(const DebugConsole&);

public:

  ~DebugConsole(){writeAndResetBuffer(); }

  static DebugConsole* instance();


  void clearBuffer(){mBuffer.clear(); flush();}


  static void flush()
  {
    if (!mDestroyed)
    {
      mFlushed = true; SendMessage(mHwnd, umSetscroll, NULL, NULL);
    }
  }

  // 将缓冲区写入日志文件，清空内容并重置滚动信息。
  void writeAndResetBuffer();

  // 启用或禁用调试输出。
  static void  off(){mActive = false;}
  static void  on()  {mActive = true;}

  bool destroyed()const{return mDestroyed;}


  // 重载输出运算符，让控制台接受可写入流的不同类型。
  template <class T>
  DebugConsole& operator<<(const T& t)
  {
    if (!mActive || mDestroyed) return *this;

    // 缓冲区超出限制时写入文件，并重置缓冲区和滚动信息。
    if (mBuffer.size() > maxBufferSize)
    {
       writeAndResetBuffer();
    }

    std::ostringstream ss; ss << t;

    if (ss.str() == ""){flush(); return *this;}

    if (!mFlushed)
      {mBuffer.back() += ss.str();}
    else
      {mBuffer.push_back(ss.str());mFlushed = false;}

    return *this;
  }
};



#endif