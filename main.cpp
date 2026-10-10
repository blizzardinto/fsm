#pragma warning (disable:4786)
#include <windows.h>
#include <time.h>

#include "constants.h"
#include "utils.h"
#include "PrecisionTimer.h"
#include "SoccerPitch.h"
#include "Cgdi.h"
#include "ParamLoader.h"
#include "Resource.h"
#include "WindowUtils.h"
#include "DebugConsole.h"


//--------------------------------- Globals ------------------------------
//
//------------------------------------------------------------------------

const wchar_t* gApplicationName = L"基于有限状态机的足球人";
const wchar_t* gWindowClassName = L"MyWindowClass";

SoccerPitch* gSoccerPitch;

//toolbar handle (unused in SimpleSoccer; referenced by Pathfinder.cpp)
HWND gToolbar;

//create a timer
PrecisionTimer timer(prm.frameRate);


//used when a user clicks on a menu item to ensure the option is 'checked'
//correctly
void checkAllMenuItemsAppropriately(HWND hwnd)
{
   checkMenuItemAppropriately(hwnd, IDM_SHOW_REGIONS, prm.bRegions);
   checkMenuItemAppropriately(hwnd, IDM_SHOW_STATES, prm.bStates);
   checkMenuItemAppropriately(hwnd, IDM_SHOW_IDS, prm.bIds);
   checkMenuItemAppropriately(hwnd, IDM_AIDS_SUPPORTSPOTS, prm.bSupportSpots);
   checkMenuItemAppropriately(hwnd, ID_AIDS_SHOWTARGETS, prm.bViewTargets);
   checkMenuItemAppropriately(hwnd, IDM_AIDS_HIGHLITE, prm.bHighlightIfThreatened);
}


//---------------------------- windowProc ---------------------------------
//
//	This is the callback function which handles all the windows messages
//-------------------------------------------------------------------------

LRESULT CALLBACK windowProc (HWND   hwnd,
                             UINT   msg,
                             WPARAM wParam,
                             LPARAM lParam)
{

   //these hold the dimensions of the client window area
   static int cxClient, cyClient;

   //used to create the back buffer
   static HDC		hdcBackBuffer;
   static HBITMAP	hBitmap;
   static HBITMAP	hOldBitmap;

    switch (msg)
    {

    //A WM_CREATE msg is sent when your application window is first
    //created
    case WM_CREATE:
      {
         //to get get the size of the client window first we need  to create
         //a RECT and then ask Windows to fill in our RECT structure with
         //the client window size. Then we assign to cxClient and cyClient
         //accordingly
         RECT rect;

         GetClientRect(hwnd, &rect);

         cxClient = rect.right;
         cyClient = rect.bottom;

         //seed random number generator
         srand((unsigned) time(NULL));


         //---------------create a surface to render to(backbuffer)

         //create a memory device context
         hdcBackBuffer = CreateCompatibleDC(NULL);

         //get the DC for the front buffer
         HDC hdc = GetDC(hwnd);

         hBitmap = CreateCompatibleBitmap(hdc,
                                          cxClient,
                                          cyClient);


         //select the bitmap into the memory device context
         hOldBitmap = (HBITMAP)SelectObject(hdcBackBuffer, hBitmap);

         //don't forget to release the DC
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

        }//end switch
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

        }//end switch

      }//end WM_KEYUP

      break;


    case WM_PAINT:
      {

         PAINTSTRUCT ps;

         BeginPaint (hwnd, &ps);

         gdi->startDrawing(hdcBackBuffer);

         gSoccerPitch->render();

         gdi->stopDrawing(hdcBackBuffer);



         //now blit backbuffer to front
         BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY);

         EndPaint (hwnd, &ps);

      }

      break;

    //has the user resized the client area?
    case WM_SIZE:
      {
        //if so we need to update our variables so that any drawing
        //we do using cxClient and cyClient is scaled accordingly
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

      //now to resize the backbuffer accordingly. First select
      //the old bitmap back into the DC
      SelectObject(hdcBackBuffer, hOldBitmap);

      //don't forget to do this or you will get resource leaks
      DeleteObject(hBitmap);

      //get the DC for the application
      HDC hdc = GetDC(hwnd);

      //create another bitmap of the same size and mode
      //as the application
      hBitmap = CreateCompatibleBitmap(hdc,
                      cxClient,
                      cyClient);

      ReleaseDC(hwnd, hdc);

      //select the new bitmap into the DC
      SelectObject(hdcBackBuffer, hBitmap);

      }

      break;

     case WM_DESTROY:
       {

         //clean up our backbuffer objects
         SelectObject(hdcBackBuffer, hOldBitmap);

         DeleteDC(hdcBackBuffer);
         DeleteObject(hBitmap);

         // kill the application, this sends a WM_QUIT message
         PostQuitMessage (0);
       }

       break;

     }//end switch

     //this is where all the messages not specifically handled by our
     //winproc are sent to be processed
     return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//-------------------------------- WinMain -------------------------------
//
//	The entry point of the windows program
//------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     szCmdLine,
                    int       iCmdShow)
{

  //handle to our window
  HWND						hWnd;

  //our window class structure
  WNDCLASSEXW    winclass;

  // first fill in the window class stucture
  winclass.cbSize        = sizeof(WNDCLASSEXW);
  winclass.style         = CS_HREDRAW | CS_VREDRAW;
  winclass.lpfnWndProc   = windowProc;
  winclass.cbClsExtra    = 0;
  winclass.cbWndExtra    = 0;
  winclass.hInstance     = hInstance;
  winclass.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
  winclass.hCursor       = LoadCursorW(NULL, MAKEINTRESOURCEW(32512)); // IDC_ARROW
  winclass.hbrBackground = NULL;
  winclass.lpszMenuName  = MAKEINTRESOURCEW(IDR_MENU1);
  winclass.lpszClassName = gWindowClassName;
  winclass.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));

  //register the window class
  if (!RegisterClassExW(&winclass))
  {
    MessageBox(NULL, "Registration Failed!", "Error", 0);

    //exit the application
    return 0;
  }

  //create the window and assign its id to hwnd
  hWnd = CreateWindowExW(NULL,                 // extended style
                         gWindowClassName,  // window class name
                         gApplicationName,  // window caption
                         WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                         GetSystemMetrics(SM_CXSCREEN)/2 - windowWidth/2,
                         GetSystemMetrics(SM_CYSCREEN)/2 - windowHeight/2,
                         windowWidth,     // initial x size
                         windowHeight,    // initial y size
                         NULL,                 // parent window handle
                         NULL,                 // window menu handle
                         hInstance,            // program instance handle
                         NULL);                // creation parameters

  //make sure the window creation has gone OK
  if(!hWnd)
  {
    MessageBox(NULL, "CreateWindowEx Failed!", "Error!", 0);
  }

  //start the timer
  timer.start();

  MSG msg;

  //enter the message loop
  bool bDone = false;

  while(!bDone)
  {

    while( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) )
    {
      if( msg.message == WM_QUIT )
      {
        // Stop loop if it's a quit message
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
      //update game states
      gSoccerPitch->update();

      //render
      redrawWindow(hWnd, true);

      Sleep(2);
    }

  }//end while

  delete gSoccerPitch;

  UnregisterClassW(gWindowClassName, winclass.hInstance);

  return msg.wParam;
}


