#define _WIN32_WINNT 0x0501  // define _WIN32_WINNT as 0x0500 or greater before including windows.h to have access to GetConsoleWindow() and SetLayeredWindowAttributes()
#define IDI_ICON 1001
#define WM_SHELLICON (WM_USER + 1)
#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <sstream>

#define ID_SHOW       2000
#define ID_EXIT        2001
#define ID_OPACITY     2002

#define ID_OPACITY_50   2003
#define ID_OPACITY_60   2004
#define ID_OPACITY_70   2005
#define ID_OPACITY_80   2006
#define ID_OPACITY_90   2007

POINT cursor;                      // Cursor position
BOOL is_hidden = false;
HINSTANCE instance;
HMENU menu_instance;
INT checked_item = GetPrivateProfileInt("Settings", "checked_item", ID_OPACITY_80, "./config.ini");

BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem ) {
      HMENU hPop = CreatePopupMenu();
      menu_instance = hPop;

      InsertMenu( hPop, 0, MF_BYPOSITION | MF_STRING, ID_SHOW, "Show/Hide");
      InsertMenu( hPop, 1, MF_BYPOSITION | MF_SEPARATOR, ID_OPACITY, "");

      for(int i=1; i<6; i++){
          std::stringstream ss;
          ss << (i+4)*10;
          std::string percentage = ss.str() + " %";
          if(checked_item == (2002+i)){
              InsertMenu( hPop, (i+1), MF_BYPOSITION | MF_CHECKED, (2002+i), percentage.c_str() );
          }else{
              InsertMenu( hPop, (i+1), MF_BYPOSITION | MF_UNCHECKED, (2002+i), percentage.c_str() );
          }
       }

      InsertMenu( hPop, 7, MF_BYPOSITION | MF_SEPARATOR, ID_OPACITY, "");
      InsertMenu( hPop, 8, MF_BYPOSITION | MF_STRING, ID_EXIT , "Exit");

      SetMenuDefaultItem(hPop, ID_SHOW, FALSE);
      SetForegroundWindow(hWnd);
      SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);

      GetCursorPos(&cursor);
      WORD cmd = TrackPopupMenu( hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, 0, hWnd, NULL );

      SendMessage( hWnd, WM_COMMAND, cmd, 0 );
    
      DestroyMenu(hPop);
      return 0;
    }

void RemoveTrayIcon( HWND hWnd, UINT uID ) {
    NOTIFYICONDATA  nid;
    nid.hWnd = hWnd;
    nid.uID  = uID;
    Shell_NotifyIcon( NIM_DELETE, &nid );
    DestroyIcon(nid.hIcon);
}

void AddTrayIcon( HWND hWnd, UINT uID, UINT uIcon ) {
      NOTIFYICONDATA  nid;

      nid.cbSize = sizeof(NOTIFYICONDATA);
      nid.hWnd = hWnd;
      nid.uID = uID;
      nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
      nid.uCallbackMessage = WM_SHELLICON;

      nid.hIcon = LoadIcon(instance, "MAINICON");
      strcpy       (nid.szTip, "The Amazing Overlay App");

      Shell_NotifyIcon( NIM_ADD, &nid );
    }

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("MyWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow){
    HWND hwnd;               // This is the handle for our window
    MSG messages;            // Here messages to the application are saved
    WNDCLASSEX wincl;        // Data structure for the windowclass
    POINT cursor;            // Cursor position
    HANDLE file = CreateFile("./config.ini", GENERIC_WRITE, 0, 0,CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

    if (file != INVALID_HANDLE_VALUE){
        CloseHandle(file);
    }else if(GetLastError() != ERROR_FILE_EXISTS){
        // file exists
    }

    GetCursorPos(&cursor);
    instance = hThisInstance;

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    // ExtractIconEx( "./icons/icon_256.ico", 0, NULL, &(wincl.hIcon), 1 );
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_WINDOWTEXT;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |  WS_EX_TOOLWINDOW,    /* Extended possibilites for variation */
           szClassName,     /* Classname */
           _T("The Amazing Overlay App"),   /* Title Text */
           WS_POPUP | WS_VISIBLE,   /* default window */
           // GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
           // GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN),
           cursor.x, cursor.y, 5, 5, /*The position on the screen to create the window and window size*/
           HWND_DESKTOP,    /* The window is a child-window to desktop */
           NULL,    /* No menu */
           hThisInstance,   /* Program Instance handler */
           NULL     /* No Window Creation data */
        );

        switch (checked_item){
            case ID_OPACITY_50:
                SetLayeredWindowAttributes(hwnd, 0, 127, LWA_ALPHA);
                checked_item = ID_OPACITY_50;
                WritePrivateProfileString("Settings", "checked_item", "2003", "./config.ini");
                break;
            case ID_OPACITY_60:
                SetLayeredWindowAttributes(hwnd, 0, 153, LWA_ALPHA);
                checked_item = ID_OPACITY_60;
                WritePrivateProfileString("Settings", "checked_item", "2004", "./config.ini");
                break;
            case ID_OPACITY_70:
                SetLayeredWindowAttributes(hwnd, 0, 178, LWA_ALPHA);
                checked_item = ID_OPACITY_70;
                WritePrivateProfileString("Settings", "checked_item", "2005", "./config.ini");
                break;
            case ID_OPACITY_80:
                SetLayeredWindowAttributes(hwnd, 0, 204, LWA_ALPHA);
                checked_item = ID_OPACITY_80;
                WritePrivateProfileString("Settings", "checked_item", "2006", "./config.ini");
                break;
            case ID_OPACITY_90:
                SetLayeredWindowAttributes(hwnd, 0, 230, LWA_ALPHA);
                checked_item = ID_OPACITY_90;
                WritePrivateProfileString("Settings", "checked_item", "2007", "./config.ini");
                break;
            default:
                break;
        }

    /* Make the window visible on the screen */
    ShowWindow (hwnd, SW_MAXIMIZE);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0)){
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    UnregisterClass(szClassName, hThisInstance);

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){   /* handle the messages */
        case WM_CREATE:
            AddTrayIcon(hwnd, 1, 0);
            break;
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
            RemoveTrayIcon(hwnd, 1);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_COMMAND:
          switch (LOWORD(wParam)) {
            case ID_SHOW:
                if(is_hidden){
                    ShowWindow (hwnd, SW_SHOW);
                    is_hidden = false;
                }else{
                    ShowWindow (hwnd, SW_HIDE);
                    is_hidden = true;
                }
                break;
            case ID_OPACITY_50:
                SetLayeredWindowAttributes(hwnd, 0, 127, LWA_ALPHA);
                checked_item = ID_OPACITY_50;
                WritePrivateProfileString("Settings", "checked_item", "2003", "./config.ini");
                break;
            case ID_OPACITY_60:
                SetLayeredWindowAttributes(hwnd, 0, 153, LWA_ALPHA);
                checked_item = ID_OPACITY_60;
                WritePrivateProfileString("Settings", "checked_item", "2004", "./config.ini");
                break;
            case ID_OPACITY_70:
                SetLayeredWindowAttributes(hwnd, 0, 178, LWA_ALPHA);
                checked_item = ID_OPACITY_70;
                WritePrivateProfileString("Settings", "checked_item", "2005", "./config.ini");
                break;
            case ID_OPACITY_80:
                SetLayeredWindowAttributes(hwnd, 0, 204, LWA_ALPHA);
                checked_item = ID_OPACITY_80;
                WritePrivateProfileString("Settings", "checked_item", "2006", "./config.ini");
                break;
            case ID_OPACITY_90:
                SetLayeredWindowAttributes(hwnd, 0, 230, LWA_ALPHA);
                checked_item = ID_OPACITY_90;
                WritePrivateProfileString("Settings", "checked_item", "2007", "./config.ini");
                break;
            case ID_EXIT:
                PostMessage( hwnd, WM_CLOSE, 0, 0 );
                break;
            default:
                return DefWindowProc (hwnd, message, wParam, lParam);
          }
          break;
        case WM_SHELLICON:
            switch (lParam){
                case WM_RBUTTONUP:
                    ShowPopupMenu(hwnd, NULL, -1 );
                    PostMessage( hwnd, WM_SHELLICON + 1, 0, 0 );
                    break;
                default:
                    return DefWindowProc (hwnd, message, wParam, lParam);
            }
           break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
