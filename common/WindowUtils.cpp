/*
 * 阅读提示：窗口工具函数，将菜单、尺寸和文件对话框操作封装成可复用接口。
 * 这些函数处理平台细节，足球业务对象不需要了解窗口消息的具体过程。
 * 本文件提供方法实现；对应头文件描述可供其他模块使用的接口。
 */
#include "WindowUtils.h"
#include <windows.h>
#include "Vector2D.h"
#include "utils.h"
#include "Stream_Utility_Functions.h"



// 根据菜单项编号和目标状态，更新指定窗口的菜单项。
void changeMenuState(HWND hwnd, UINT menuItem, UINT state)
{
  MENUITEMINFO mi;

  mi.cbSize = sizeof(MENUITEMINFO);
  mi.fMask  = MIIM_STATE;
  mi.fState = state;

  SetMenuItemInfo(GetMenu(hwnd), menuItem, false, &mi);
  DrawMenuBar(hwnd);
}

// 布尔值为真时勾选菜单项，否则取消勾选。
void checkMenuItemAppropriately(HWND hwnd, UINT menuItem, bool b)
{
  if (b)
  {
    changeMenuState(hwnd, menuItem, MFS_CHECKED);
  }
  else
  {
    changeMenuState(hwnd, menuItem, MFS_UNCHECKED);
  }
}


// 检查字符串缓冲区长度，提供旧版工具栏代码所需的长度检查接口。
bool checkBufferLength(char* buff, int maxLength, int& bufferLength)
{
  std::string s = ttos(buff);

  bufferLength = s.length();

  if (bufferLength > maxLength)
  {
    bufferLength = 0; return false;
  }

  return true;
}

void errorBox(std::string& msg)
{
  MessageBox(NULL, msg.c_str(), "Error", MB_OK);
}

void errorBox(char* msg)
{
  MessageBox(NULL, msg, "Error", MB_OK);
}

// 获取鼠标相对于活动窗口的位置。
Vector2D getClientCursorPosition()
{
  POINT mousePos;

  GetCursorPos(&mousePos);

  ScreenToClient(GetActiveWindow(), &mousePos);

  return pointToVector(mousePos);
}


Vector2D getClientCursorPosition(HWND hwnd)
{
  POINT mousePos;

  GetCursorPos(&mousePos);

  ScreenToClient(hwnd, &mousePos);

  return pointToVector(mousePos);
}


// 以下文件对话框辅助函数参考 Petzold 的著作。
void fileInitialize (HWND hwnd,
                     OPENFILENAME& ofn,
                     const std::string& defaultFileTypeDescription,
                     const std::string& defaultFileExtension)
{
  std::string filter = defaultFileTypeDescription + '\0' + "*." + defaultFileExtension + '\0' +
                       "All Files (*.*)" + '\0' + "*.*" + '\0' + '\0';

   static TCHAR szFilter[255];

   for (unsigned int i=0; i<filter.size(); ++i)
   {
     szFilter[i] = filter.at(i);
   }

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;          // 该字段由打开或保存对话框函数设置。
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // 该字段由打开或保存对话框函数设置。
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // 该字段由打开或保存对话框函数设置。
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = defaultFileExtension.c_str() ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;

}



BOOL fileOpenDlg (HWND               hwnd,
                  PTSTR              pstrFileName,
                  PTSTR              pstrTitleName,
                  const std::string& defaultFileTypeDescription,
                  const std::string& defaultFileExtension)
{
     OPENFILENAME ofn;

     fileInitialize(hwnd, ofn, defaultFileTypeDescription, defaultFileExtension);

     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

     return GetOpenFileName (&ofn) ;
}

BOOL fileSaveDlg (HWND               hwnd,
                  PTSTR              pstrFileName,
                  PTSTR              pstrTitleName,
                  const std::string& defaultFileTypeDescription,
                  const std::string& defaultFileExtension)
{
     OPENFILENAME ofn; fileInitialize(hwnd, ofn, defaultFileTypeDescription, defaultFileExtension);

     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_OVERWRITEPROMPT ;

     return GetSaveFileName (&ofn) ;
}

// 将窗口调整到指定的客户区大小。
void resizeWindow(HWND hwnd, int cx, int cy)
{
  // 判断窗口是否带菜单。
  HMENU hwndMenu = GetMenu(hwnd);
  bool bMenu = false;
  if (hwndMenu) bMenu = true;

  // 用矩形描述期望的客户区尺寸。
  RECT desiredSize;
  desiredSize.left = 0;
  desiredSize.top  = 0;
  desiredSize.right = cx;
  desiredSize.bottom = cy;

  // 根据边框、标题栏和菜单计算实际窗口尺寸。
  AdjustWindowRectEx(&desiredSize,
                     WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                     bMenu,
                     NULL);

  // 调整窗口大小。
  SetWindowPos(hwnd,
               NULL,
               GetSystemMetrics(SM_CXSCREEN)/2 - cx/2,
               GetSystemMetrics(SM_CYSCREEN)/2 - cy/2,
               desiredSize.right,
               desiredSize.bottom,
               SWP_NOZORDER);
}

// 返回窗口高度。
int  getWindowHeight(HWND hwnd)
{
  if (hwnd == 0) return 0;

  RECT windowRect;

  GetWindowRect(hwnd, &windowRect);

  return windowRect.bottom - windowRect.top;
}

// 返回窗口宽度。
int  getWindowWidth(HWND hwnd)
{
  if (hwnd == 0) return 0;

  RECT windowRect;

  GetWindowRect(hwnd, &windowRect);

  return windowRect.right - windowRect.left;
}