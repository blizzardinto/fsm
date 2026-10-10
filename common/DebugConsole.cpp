/*
 * 阅读提示：调试输出组件，封装窗口、缓冲区和日志文件。
 * 输出运算符让调用代码像写标准输出一样记录信息；禁用时使用相同接口的空接收器。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "DebugConsole.h"
#include <iterator>
#pragma warning (disable : 4786)

// 定义并初始化类的静态成员；静态成员由所有实例共享。
std::vector<std::string> DebugConsole::mBuffer;
HWND                     DebugConsole::mHwnd       = NULL;
bool                     DebugConsole::mFlushed   = true;
bool                     DebugConsole::mDestroyed = false;
bool                     DebugConsole::mActive   = true;
std::ofstream            DebugConsole::mLogOut;
int                      DebugConsole::mPosLeft;
int                      DebugConsole::mPosTop;




// 调试窗口的消息回调，负责滚动和文字绘制。
LRESULT CALLBACK DebugConsole::debugWindowProc(HWND hwnd,
                                         UINT msg,
                                         WPARAM wparam,
                                         LPARAM lparam)
{
  // 保存调试窗口的客户区尺寸。
  static int cxClient, cyClient;

  // 保存字体字符的宽高。
  static int cxChar, cyChar, cxCaps, cyPage;

  int iVertPos;

  TEXTMETRIC  tm;
  SCROLLINFO  si;

  // 获取窗口客户区大小。
  RECT rect;
  GetClientRect(hwnd, &rect);
  cxClient = rect.right;
  cyClient = rect.bottom;

  switch(msg)
  {
    case WM_CREATE:
    {
      // 获取字体度量，计算一行文字需要的空间。
      HDC hdc = GetDC(hwnd);

      GetTextMetrics(hdc, &tm);
      cxChar = tm.tmAveCharWidth;
      cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
      cyChar = tm.tmHeight + tm.tmExternalLeading;

      // 设置垂直滚动条的范围和页面大小。
      si.cbSize = sizeof (si) ;
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0 ;
      si.nMax   = 10 ;
      si.nPage  = cyClient / cyChar;

      SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;

      ReleaseDC(hwnd, hdc);

    }

    break;

    case WM_KEYUP:
      {
        switch(wparam)
        {
           case VK_ESCAPE:
            {
              SendMessage(hwnd, WM_DESTROY, NULL, NULL);
            }

            break;
        }
      }

      break;


    case WM_VSCROLL:

      // 读取垂直滚动条信息。
      si.cbSize = sizeof (si);
      si.fMask  = SIF_ALL;
      GetScrollInfo (hwnd, SB_VERT, &si) ;

      // 保存原位置，稍后判断滚动位置是否改变。
      iVertPos = si.nPos ;

      switch (LOWORD (wparam))
      {
        case SB_TOP:
             si.nPos = si.nMin ;
             break ;

        case SB_BOTTOM:
             si.nPos = si.nMax ;
             break ;

        case SB_LINEUP:
             si.nPos -= 1 ;
             break ;

        case SB_LINEDOWN:
             si.nPos += 1 ;
             break ;

        case SB_PAGEUP:
             si.nPos -= si.nPage ;
             break ;

        case SB_PAGEDOWN:
             si.nPos += si.nPage ;
             break ;

        case SB_THUMBTRACK:
             si.nPos = si.nTrackPos ;
             break ;

        default:
             break ;
      }

      // 设置滚动位置后重新读取；Windows 可能对超出范围的值进行调整。
      si.fMask = SIF_POS ;
      SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;
      GetScrollInfo (hwnd, SB_VERT, &si) ;

      // 位置改变时滚动并刷新窗口。
      if (si.nPos != iVertPos)
      {
         ScrollWindow (hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL) ;
         UpdateWindow (hwnd) ;
         drawWindow();
      }

      break ;


    case umSetscroll:
    {
      // 读取垂直滚动条信息。
      si.cbSize = sizeof (si);
      si.fMask  = SIF_ALL;
      GetScrollInfo (hwnd, SB_VERT, &si);

      si.nMin = 0;
      si.nMax = mBuffer.size();

      si.nPos += 1;

      si.fMask  = SIF_RANGE | SIF_POS;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

      ScrollWindow (hwnd, 0, cyChar * si.nPos, NULL, NULL);
      drawWindow();
    }

    break;

    case WM_PAINT:
    {
      PAINTSTRUCT ps;

      BeginPaint(hwnd, &ps);

        SetBkMode(ps.hdc, TRANSPARENT);
        SetTextColor(ps.hdc, RGB(255,255,255));

        if (mBuffer.size() > 1)
        {

          // 获取当前垂直滚动位置。
          si.cbSize = sizeof (si) ;
          si.fMask  = SIF_POS ;
          GetScrollInfo (hwnd, SB_VERT, &si) ;
          iVertPos = si.nPos ;

          // 计算当前页面能够显示的行数。
          int pageSize = (int)(cyClient / cyChar) - 1;

          int startIndex = 0;

          if (iVertPos > pageSize)
          {
            startIndex = iVertPos - pageSize;
          }

          std::vector<std::string>::iterator beg = mBuffer.begin() + startIndex;
          std::vector<std::string>::iterator end = mBuffer.begin() + startIndex+pageSize+1;

          int line=0;

          for (beg; (beg !=end) && (beg != mBuffer.end()); ++beg)
          {
            TextOut(ps.hdc, 0, cyChar*line++, (*beg).c_str(), (*beg).size());
          }
        }

      EndPaint(hwnd, &ps);
    }

    break;

    case WM_SIZE:
      {
      }
      break;

    case WM_DESTROY:
    {
      mDestroyed = true;

      DestroyWindow(mHwnd);
     }

    break;

    default:break;

  }// 未处理的消息交给默认窗口过程。
  if (!mDestroyed)
  {
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
  else
  {
    return NULL;
  }

}


// 创建调试窗口及日志文件。
bool DebugConsole::create()
{
  mHwnd       = NULL;
  mPosLeft   = 0;
  mPosTop    = 0;
  mFlushed   = true;

  // 打开日志文件。
  mLogOut.open("DebugLog.txt");


  WNDCLASSEX wDebugConsole = {sizeof(WNDCLASSEX),
                       CS_HREDRAW | CS_VREDRAW,
                       debugWindowProc,
                       0,
                       0,
                       GetModuleHandle(NULL),
                       NULL,
                       NULL,
                       (HBRUSH)(GetStockObject(GRAY_BRUSH)),
                       NULL,
                       "Debug",
                       NULL };


  // 注册调试窗口类。
  if (!RegisterClassEx(&wDebugConsole))
  {
    MessageBox(NULL, "Registration of Debug Console Failed!", "Error", 0);

    // 注册失败时结束程序。
    return false;
  }


  // 创建调试信息窗口。
  mHwnd = CreateWindow("Debug",
                            "Debug Console",
                            WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU| WS_VSCROLL | WS_THICKFRAME,
                            0,
                            0,
                            debugWindowWidth,
                            debugWindowHeight,
                            NULL,
                            NULL,
                            wDebugConsole.hInstance,
                            NULL );

    // 检查窗口是否创建成功。
  if(!mHwnd)
  {
    MessageBox(mHwnd, "CreateWindowEx Failed!", "Error!", 0);

    return false;
  }

  // 显示窗口。
  UpdateWindow(mHwnd);

  return true;

}

// 返回调试控制台的单例对象指针。
DebugConsole* DebugConsole::instance()
{
   static DebugConsole instance;
   static bool created = false;
   if ( !created)
   {create();created = true;}

   return &instance;
}

// 将缓冲区写入日志，然后清空并重置滚动信息。
void DebugConsole::writeAndResetBuffer()
{

  mPosLeft   = 0;
  mPosTop    = 0;
  mFlushed   = true;

  // 把缓冲区内容写入文件。
  std::vector<std::string>::iterator it = mBuffer.begin();

  for (it; it != mBuffer.end(); ++it)
  {
    mLogOut << *it << std::endl;
  }

  mBuffer.clear();

  SendMessage(mHwnd, umSetscroll, NULL, NULL);
}