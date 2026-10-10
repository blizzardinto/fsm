/*
 * 阅读提示：程序入口和窗口事件适配层。它创建球场对象，并按计时器驱动更新和绘制。
 * 阅读时从 WinMain 的主循环出发，再跟进 SoccerPitch::update，观察对象如何逐层协作。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#pragma warning (disable:4786)
#include <windows.h>
#include <time.h>

#include "constants.h"
#include "utils.h"
#include "PrecisionTimer.h"
#include "SoccerPitch.h"
#include "Cgdi.h"
#include "ParamLoader.h"
#include "res/resource.h"
#include "WindowUtils.h"
#include "DebugConsole.h"


// 全局对象：窗口主循环共同使用的比赛、计时器和绘图状态。

const wchar_t* gApplicationName = L"基于有限状态机的足球人";
const wchar_t* gWindowClassName = L"MyWindowClass";

SoccerPitch* gSoccerPitch;

// 创建计时器，用来控制每秒更新的次数。
PrecisionTimer timer(prm.frameRate);


// 菜单命令与勾选状态配合，保持界面显示和实际选项一致。
void checkAllMenuItemsAppropriately(HWND hwnd)
{
   checkMenuItemAppropriately(hwnd, IDM_SHOW_REGIONS, prm.bRegions);
   checkMenuItemAppropriately(hwnd, IDM_SHOW_STATES, prm.bStates);
   checkMenuItemAppropriately(hwnd, IDM_SHOW_IDS, prm.bIds);
   checkMenuItemAppropriately(hwnd, IDM_AIDS_SUPPORTSPOTS, prm.bSupportSpots);
   checkMenuItemAppropriately(hwnd, ID_AIDS_SHOWTARGETS, prm.bViewTargets);
   checkMenuItemAppropriately(hwnd, IDM_AIDS_HIGHLITE, prm.bHighlightIfThreatened);
}


// 窗口消息回调：Windows 把创建、键盘、绘图等事件交给这个函数处理。

LRESULT CALLBACK windowProc (HWND   hwnd,
                             UINT   msg,
                             WPARAM wParam,
                             LPARAM lParam)
{

   // 保存客户区宽高；客户区不包含标题栏和边框。
   static int cxClient, cyClient;

   // 双缓冲绘图：先画到内存位图，再一次性显示，减少闪烁。
   static HDC		hdcBackBuffer;
   static HBITMAP	hBitmap;
   static HBITMAP	hOldBitmap;

    switch (msg)
    {

    // 窗口首次创建时，Windows 会发送 WM_CREATE 消息。
    case WM_CREATE:
      {
         // 通过 RECT 接收客户区边界，再计算宽度和高度。
         RECT rect;

         GetClientRect(hwnd, &rect);

         cxClient = rect.right;
         cyClient = rect.bottom;

         // 初始化随机数种子，让每次比赛的随机行为有所不同。
         srand((unsigned) time(NULL));


         // 创建内存设备上下文，作为离屏绘图的画布。
         hdcBackBuffer = CreateCompatibleDC(NULL);

         // 获取窗口设备上下文，用它创建兼容的内存位图。
         HDC hdc = GetDC(hwnd);

         hBitmap = CreateCompatibleBitmap(hdc,
                                          cxClient,
                                          cyClient);


         // 把位图选入内存设备上下文，后续绘图就落在该位图上。
         hOldBitmap = (HBITMAP)SelectObject(hdcBackBuffer, hBitmap);

         // 获取的窗口设备上下文需要及时释放。
         ReleaseDC(hwnd, hdc);

         gSoccerPitch = new SoccerPitch(cxClient, cyClient);

         checkAllMenuItemsAppropriately(hwnd);

      }

      break;

    case WM_COMMAND:
      {
        switch(wParam)
        {
          case ID_AIDS_NOAIDS:

            prm.bStates        = 0;
            prm.bRegions       = 0;
            prm.bIds           = 0;
            prm.bSupportSpots  = 0;
            prm.bViewTargets   = 0;

            checkAllMenuItemsAppropriately(hwnd);

            break;

          case IDM_SHOW_REGIONS:

            prm.bRegions = !prm.bRegions;

            checkAllMenuItemsAppropriately(hwnd);

            break;

          case IDM_SHOW_STATES:

            prm.bStates = !prm.bStates;

            checkAllMenuItemsAppropriately(hwnd);

            break;

          case IDM_SHOW_IDS:

            prm.bIds = !prm.bIds;

            checkAllMenuItemsAppropriately(hwnd);

            break;


          case IDM_AIDS_SUPPORTSPOTS:

            prm.bSupportSpots = !prm.bSupportSpots;

            checkAllMenuItemsAppropriately(hwnd);

             break;

           case ID_AIDS_SHOWTARGETS:

            prm.bViewTargets = !prm.bViewTargets;

            checkAllMenuItemsAppropriately(hwnd);

             break;

           case IDM_AIDS_HIGHLITE:

            prm.bHighlightIfThreatened = !prm.bHighlightIfThreatened;

            checkAllMenuItemsAppropriately(hwnd);

            break;

        }// 结束菜单命令的分支处理。
      }

      break;


    case WM_KEYUP:
      {
        switch(wParam)
        {
           case VK_ESCAPE:
            {
              SendMessage(hwnd, WM_DESTROY, NULL, NULL);
            }

            break;

          case 'R':
            {
               delete gSoccerPitch;

               gSoccerPitch = new SoccerPitch(cxClient, cyClient);
            }

            break;

          case 'P':
            {
              gSoccerPitch->togglePause();
            }

            break;

        }// 结束按键的分支处理。

      }// 结束键盘松开事件的处理。

      break;


    case WM_PAINT:
      {

         PAINTSTRUCT ps;

         BeginPaint (hwnd, &ps);

         gdi->startDrawing(hdcBackBuffer);

         gSoccerPitch->render();

         gdi->stopDrawing(hdcBackBuffer);



         // 将已经画好的内存位图复制到窗口上。
         BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY);

         EndPaint (hwnd, &ps);

      }

      break;

    // 判断客户区大小是否发生变化。
    case WM_SIZE:
      {
        // 更新客户区尺寸，供后续绘图使用。
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

      // 重建缓冲位图之前，先把原来的位图选回设备上下文。
      SelectObject(hdcBackBuffer, hOldBitmap);

      // 删除不再使用的位图，防止绘图资源泄漏。
      DeleteObject(hBitmap);

      // 获取窗口的设备上下文。
      HDC hdc = GetDC(hwnd);

      // 按新的客户区尺寸创建兼容位图。
      hBitmap = CreateCompatibleBitmap(hdc,
                      cxClient,
                      cyClient);

      ReleaseDC(hwnd, hdc);

      // 将新位图选入内存设备上下文。
      SelectObject(hdcBackBuffer, hBitmap);

      }

      break;

     case WM_DESTROY:
       {

         // 释放双缓冲使用的位图和设备上下文。
         SelectObject(hdcBackBuffer, hOldBitmap);

         DeleteDC(hdcBackBuffer);
         DeleteObject(hBitmap);

         // 请求退出程序，向消息队列发送 WM_QUIT。
         PostQuitMessage (0);
       }

       break;

     }// 未处理的消息交给 Windows 默认窗口过程。
     return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Windows 程序入口：建立窗口，启动计时器，然后进入消息与比赛循环。
int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     szCmdLine,
                    int       iCmdShow)
{

  // 保存窗口句柄；句柄是 Windows 用来标识资源的编号。
  HWND						hWnd;

  // 窗口类描述窗口使用的回调、图标和鼠标样式。
  WNDCLASSEXW    winclass;

  // 填写窗口类配置。
  winclass.cbSize        = sizeof(WNDCLASSEXW);
  winclass.style         = CS_HREDRAW | CS_VREDRAW;
  winclass.lpfnWndProc   = windowProc;
  winclass.cbClsExtra    = 0;
  winclass.cbWndExtra    = 0;
  winclass.hInstance     = hInstance;
  winclass.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
  winclass.hCursor       = LoadCursorW(NULL, MAKEINTRESOURCEW(32512)); // 使用系统箭头光标。
  winclass.hbrBackground = NULL;
  winclass.lpszMenuName  = MAKEINTRESOURCEW(IDR_MENU1);
  winclass.lpszClassName = gWindowClassName;
  winclass.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));

  // 向 Windows 注册窗口类。
  if (!RegisterClassExW(&winclass))
  {
    MessageBox(NULL, "Registration Failed!", "Error", 0);

    // 注册失败时结束程序。
    return 0;
  }

  // 创建窗口，并保存它的句柄。
  hWnd = CreateWindowExW(NULL,                 // 扩展窗口样式。
                         gWindowClassName,  // 已注册的窗口类名称。
                         gApplicationName,  // 窗口标题。
                         WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                         GetSystemMetrics(SM_CXSCREEN)/2 - windowWidth/2,
                         GetSystemMetrics(SM_CYSCREEN)/2 - windowHeight/2,
                         windowWidth,     // 初始窗口宽度。
                         windowHeight,    // 初始窗口高度。
                         NULL,                 // 父窗口句柄；空值表示没有父窗口。
                         NULL,                 // 菜单句柄。
                         hInstance,            // 当前程序实例句柄。
                         NULL);                // 创建时的附加参数；随后检查窗口是否创建成功。
  if(!hWnd)
  {
    MessageBox(NULL, "CreateWindowEx Failed!", "Error!", 0);
  }

  // 在主循环开始前启动计时器。
  timer.start();

  MSG msg;

  // 消息循环同时驱动窗口事件和比赛更新。
  bool bDone = false;

  while(!bDone)
  {

    while( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) )
    {
      if( msg.message == WM_QUIT )
      {
        // 收到退出消息后跳出循环。
        bDone = true;
      }

      else
      {
        TranslateMessage( &msg );
        DispatchMessageW( &msg );
      }
    }

    if (timer.readyForNextFrame() && msg.message != WM_QUIT)
    {
      // 更新比赛中的球队、球员和足球。
      gSoccerPitch->update();

      // 请求绘制当前比赛画面。
      redrawWindow(hWnd, true);

      Sleep(2);
    }

  }// 主循环结束。

  delete gSoccerPitch;

  UnregisterClassW(gWindowClassName, winclass.hInstance);

  return msg.wParam;
}


