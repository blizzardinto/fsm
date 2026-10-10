/*
 * 阅读提示：窗口工具函数，将菜单、尺寸和文件对话框操作封装成可复用接口。
 * 这些函数处理平台细节，足球业务对象不需要了解窗口消息的具体过程。
 * 阅读接口时先看类的职责，再看公开方法，最后看内部成员和实现。
 */
#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H
#pragma warning (disable:4786)

#include <windows.h>
#include <string>

struct Vector2D;

// 检查按键是否处于按下状态的宏。
#define KEYDOWN(vkCode) ((GetAsyncKeyState(vkCode) & 0x8000) ? 1 : 0)

#define WAS_KEY_PRESSED(vkCode) ((GetKeyState(vkCode) & 0x8000) != 0)
#define IS_KEY_PRESSED(vkCode) ((GetAsyncKeyState(vkCode) & 0x8000) != 0)

// 请求刷新客户区。
inline void redrawWindow(HWND hwnd, bool redrawBackGround = true)
{
  InvalidateRect(hwnd, NULL, redrawBackGround);
  UpdateWindow(hwnd);
}

// 请求刷新客户区。
inline void redrawWindowRect(HWND hwnd, bool redrawBackGround, RECT& redrawArea)
{
  InvalidateRect(hwnd, &redrawArea, redrawBackGround);
  UpdateWindow(hwnd);
}


// 根据菜单项编号、目标状态和窗口句柄更新菜单项。
void changeMenuState(HWND hwnd, UINT menuItem, UINT state);

// 布尔值为真时勾选菜单项，否则取消勾选。
void checkMenuItemAppropriately(HWND hwnd, UINT menuItem, bool b);


// 检查字符串缓冲区长度，兼容原有窗口工具代码。
bool checkBufferLength(char* buff, int maxLength, unsigned int& bufferLength);

void errorBox(std::string& msg);
void errorBox(char* msg);

// 获取鼠标相对于活动窗口的位置。
Vector2D getClientCursorPosition();

// 获取鼠标相对于活动窗口的位置。
Vector2D getClientCursorPosition(HWND hwnd);


// 打开系统文件对话框获取文件名；实现参考 Petzold 的著作。
void fileInitialize (HWND hwnd,
                     OPENFILENAME& ofn,
                     const std::string& defaultFileTypeDescription,
                     const std::string& defaultFileExtension);

BOOL fileOpenDlg (HWND               hwnd,
                  PTSTR              pstrFileName,
                  PTSTR              pstrTitleName,
                  const std::string& defaultFileTypeDescription,
                  const std::string& defaultFileExtension);

BOOL fileSaveDlg (HWND hwnd,
                  PTSTR pstrFileName,
                  PTSTR pstrTitleName,
                  const std::string& defaultFileTypeDescription,
                  const std::string& defaultFileExtension);

// 将指定窗口调整到指定客户区大小。
void resizeWindow(HWND hwnd, int cx, int cy);

int  getWindowHeight(HWND hwnd);
int  getWindowWidth(HWND hwnd);




#endif