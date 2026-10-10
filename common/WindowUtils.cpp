#include "WindowUtils.h"
#include <windows.h>
#include "Vector2D.h"
#include "utils.h"
#include "Stream_Utility_Functions.h"



//---------------------- changeMenuState ---------------------------------
//
//  Changes the state of a menu item given the item identifier, the
//  desired state and the HWND of the menu owner
//------------------------------------------------------------------------
void changeMenuState(HWND hwnd, UINT menuItem, UINT state)
{
  MENUITEMINFO mi;

  mi.cbSize = sizeof(MENUITEMINFO);
  mi.fMask  = MIIM_STATE;
  mi.fState = state;

  SetMenuItemInfo(GetMenu(hwnd), menuItem, false, &mi);
  DrawMenuBar(hwnd);
}

//-------------------- checkMenuItemAppropriately ----------------------------
//
//  if b is true menuItem is checked, otherwise it is unchecked
//-----------------------------------------------------------------------------
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


//--------------------- checkBufferLength --------------------------------
//
//  this is a replacement for the StringCchLength function found in the
//  platform SDK. See MSDN for details. Only ever used for checking toolbar
//  strings
//------------------------------------------------------------------------
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

//gets the coordinates of the cursor relative to an active window
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


//-----------------------------------------------------------------------------
//
//  The following 3 functions are taken from Petzold's book and enable the
//  client to use the file dialog common control
//-----------------------------------------------------------------------------
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
     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
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

//-------------------------- resizeWindow -------------------------------------
//
//  call this to resize the active window to the specified size
//-----------------------------------------------------------------------------
void resizeWindow(HWND hwnd, int cx, int cy)
{
  //does this window have a menu. If so set a flag to true
  HMENU hwndMenu = GetMenu(hwnd);
  bool bMenu = false;
  if (hwndMenu) bMenu = true;

  //create a rect of the desired window size
  RECT desiredSize;
  desiredSize.left = 0;
  desiredSize.top  = 0;
  desiredSize.right = cx;
  desiredSize.bottom = cy;

  //determine the size the window should be given the desired client area
  AdjustWindowRectEx(&desiredSize,
                     WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                     bMenu,
                     NULL);

  //resize the window to fit
  SetWindowPos(hwnd,
               NULL,
               GetSystemMetrics(SM_CXSCREEN)/2 - cx/2,
               GetSystemMetrics(SM_CYSCREEN)/2 - cy/2,
               desiredSize.right,
               desiredSize.bottom,
               SWP_NOZORDER);
}

//------------------------- getWindowHeight -----------------------------------
//-----------------------------------------------------------------------------
int  getWindowHeight(HWND hwnd)
{
  if (hwnd == 0) return 0;

  RECT windowRect;

  GetWindowRect(hwnd, &windowRect);

  return windowRect.bottom - windowRect.top;
}

//------------------------- getWindowWidth  -----------------------------------
//-----------------------------------------------------------------------------
int  getWindowWidth(HWND hwnd)
{
  if (hwnd == 0) return 0;

  RECT windowRect;

  GetWindowRect(hwnd, &windowRect);

  return windowRect.right - windowRect.left;
}