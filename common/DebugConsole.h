#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H
#pragma warning (disable:4786)
#include <vector>
#include <windows.h>
#include <iosfwd>
#include <fstream>

#include "utils.h"
#include "WindowUtils.h"


//need to define a custom message
const int umSetscroll = WM_USER + 32;

//maximum number of lines shown in console before the buffer is flushed to
//a file
const int maxBufferSize = 500;

//initial dimensions of the console window
const int debugWindowWidth  = 400;
const int debugWindowHeight = 400;

//undefine DEBUG to send all debug messages to hyperspace (a sink - see below)
//#define DEBUG
#ifdef DEBUG
#define debugCon *(DebugConsole::instance())
#else
#define debugCon *(CSink::instance())
#endif

//use these in your code to toggle output to the console on/off
#define debugOn  DebugConsole::on();
#define debugOff DebugConsole::off();


//this little class just acts as a sink for any input. Used in place
//of the DebugConsole class when the console is not required
class CSink
{
private:

  CSink(){};

  //copy ctor and assignment should be private
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

  //the string buffer. All input to debug stream is stored here
  static std::vector<std::string> mBuffer;

  //if true the next input will be pushed into the buffer. If false,
  //it will be appended.
  static bool          mFlushed;

  //position of debug window
  static int           mPosTop;
  static int           mPosLeft;

  //set to true if the window is destroyed
  static bool          mDestroyed;

  //if false the console will just disregard any input
  static bool          mActive;

  //default logging file
  static std::ofstream mLogOut;



  //the debug window message handler
  static LRESULT CALLBACK debugWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  //this registers the window class and creates the window(called by the ctor)
  static bool             create();

  static void             drawWindow(){InvalidateRect(mHwnd, NULL, TRUE); UpdateWindow(mHwnd);}

private:

  DebugConsole(){}

  //copy ctor and assignment should be private
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

  //writes the contents of the buffer to the file "debug_log.txt", clears
  //the buffer and resets the appropriate scroll info
  void writeAndResetBuffer();

  //use to activate deactivate
  static void  off(){mActive = false;}
  static void  on()  {mActive = true;}

  bool destroyed()const{return mDestroyed;}


  //overload the << to accept any type
  template <class T>
  DebugConsole& operator<<(const T& t)
  {
    if (!mActive || mDestroyed) return *this;

    //reset buffer and scroll info if it overflows. write the excess
    //to file
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