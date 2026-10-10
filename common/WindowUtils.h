#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H
#pragma warning (disable:4786)

#include <windows.h>
#include <string>

struct Vector2D;

//macro to detect keypresses
#define KEYDOWN(vkCode) ((GetAsyncKeyState(vkCode) & 0x8000) ? 1 : 0)

#define WAS_KEY_PRESSED(vkCode) ((GetKeyState(vkCode) & 0x8000) != 0)
#define IS_KEY_PRESSED(vkCode) ((GetAsyncKeyState(vkCode) & 0x8000) != 0)

//Call this to refresh the client window
inline void redrawWindow(HWND hwnd, bool redrawBackGround = true)
{
  InvalidateRect(hwnd, NULL, redrawBackGround);
  UpdateWindow(hwnd);
}

//Call this to refresh the client window
inline void redrawWindowRect(HWND hwnd, bool redrawBackGround, RECT& redrawArea)
{
  InvalidateRect(hwnd, &redrawArea, redrawBackGround);
  UpdateWindow(hwnd);
}


//Changes the state of a menu item given the item identifier, the
//desired state and the HWND of the menu owner
void changeMenuState(HWND hwnd, UINT menuItem, UINT state);

//if b is true menuItem is checked, otherwise it is unchecked
void checkMenuItemAppropriately(HWND hwnd, UINT menuItem, bool b);


//this is a replacement for the StringCchLength function found in the
//platform SDK. See MSDN for details. Only ever used for checking toolbar
//strings
bool checkBufferLength(char* buff, int maxLength, unsigned int& bufferLength);

void errorBox(std::string& msg);
void errorBox(char* msg);

//gets the coordinates of the cursor relative to an active window
Vector2D getClientCursorPosition();

//gets the coordinates of the cursor relative to an active window
Vector2D getClientCursorPosition(HWND hwnd);


//two handy functions from Mr Petzold. They open a common dialog box to
//grab a filename
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

//call this to resize the specified window to the specified size.
void resizeWindow(HWND hwnd, int cx, int cy);

int  getWindowHeight(HWND hwnd);
int  getWindowWidth(HWND hwnd);




#endif