#include "DebugConsole.h"
#include <iterator>
#pragma warning (disable : 4786)

//initialize static variable
std::vector<std::string> DebugConsole::mBuffer;
HWND                     DebugConsole::mHwnd       = NULL;
bool                     DebugConsole::mFlushed   = true;
bool                     DebugConsole::mDestroyed = false;
bool                     DebugConsole::mActive   = true;
std::ofstream            DebugConsole::mLogOut;
int                      DebugConsole::mPosLeft;
int                      DebugConsole::mPosTop;




//-----------------------------------InfoWinProc-----------------------------
//
//-----------------------------------------------------------------------
LRESULT CALLBACK DebugConsole::debugWindowProc(HWND hwnd,
                                         UINT msg,
                                         WPARAM wparam,
                                         LPARAM lparam)
{
  //these hold the dimensions of the client window area
  static int cxClient, cyClient;

  //font dimensions
  static int cxChar, cyChar, cxCaps, cyPage;

  int iVertPos;

  TEXTMETRIC  tm;
  SCROLLINFO  si;

  //get the size of the client window
  RECT rect;
  GetClientRect(hwnd, &rect);
  cxClient = rect.right;
  cyClient = rect.bottom;

  switch(msg)
  {
    case WM_CREATE:
    {
      //get the font info
      HDC hdc = GetDC(hwnd);

      GetTextMetrics(hdc, &tm);
      cxChar = tm.tmAveCharWidth;
      cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
      cyChar = tm.tmHeight + tm.tmExternalLeading;

      // Set vertical scroll bar range and page size
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

      //Get all the vertical scroll bar information
      si.cbSize = sizeof (si);
      si.fMask  = SIF_ALL;
      GetScrollInfo (hwnd, SB_VERT, &si) ;

      // save the position for comparison later on
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

      //Set the position and then retrieve it.  Due to adjustments
      //by Windows it may not be the same as the value set.
      si.fMask = SIF_POS ;
      SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;
      GetScrollInfo (hwnd, SB_VERT, &si) ;

      // If the position has changed, scroll the window and update it
      if (si.nPos != iVertPos)
      {
         ScrollWindow (hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL) ;
         UpdateWindow (hwnd) ;
         drawWindow();
      }

      break ;


    case umSetscroll:
    {
      //Get all the vertical scroll bar information
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

          // Get vertical scroll bar position
          si.cbSize = sizeof (si) ;
          si.fMask  = SIF_POS ;
          GetScrollInfo (hwnd, SB_VERT, &si) ;
          iVertPos = si.nPos ;

          //number of lines we can fit on this page
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

  }//end switch

  // default msg handler
  if (!mDestroyed)
  {
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
  else
  {
    return NULL;
  }

}


//----------------------------- create -----------------------------------
//
//------------------------------------------------------------------------
bool DebugConsole::create()
{
  mHwnd       = NULL;
  mPosLeft   = 0;
  mPosTop    = 0;
  mFlushed   = true;

  //open log file
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


  //register the window class
  if (!RegisterClassEx(&wDebugConsole))
  {
    MessageBox(NULL, "Registration of Debug Console Failed!", "Error", 0);

    //exit the application
    return false;
  }


  //get the size of the client window
 // RECT rectActive;
 // GetClientRect(GetActiveWindow(), &rectActive);

  // create the info window
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

    //make sure the window creation has gone OK
  if(!mHwnd)
  {
    MessageBox(mHwnd, "CreateWindowEx Failed!", "Error!", 0);

    return false;
  }

  // Show the window
  UpdateWindow(mHwnd);

  return true;

}

//---------------------------- instance ---------------------------------------
//
//  Retrieve a pointer to an instance of this class
//-----------------------------------------------------------------------------
DebugConsole* DebugConsole::instance()
{
   static DebugConsole instance;
   static bool created = false;
   if ( !created)
   {create();created = true;}

   return &instance;
}

//--------------------------- writeAndResetBuffer -----------------------------
//-----------------------------------------------------------------------------
void DebugConsole::writeAndResetBuffer()
{

  mPosLeft   = 0;
  mPosTop    = 0;
  mFlushed   = true;

  //write out the contents of the buffer to a file
  std::vector<std::string>::iterator it = mBuffer.begin();

  for (it; it != mBuffer.end(); ++it)
  {
    mLogOut << *it << std::endl;
  }

  mBuffer.clear();

  SendMessage(mHwnd, umSetscroll, NULL, NULL);
}