#define _WIN32_WINNT 0x0501 // define _WIN32_WINNT as 0x0500 or greater before including windows.h to have access to GetConsoleWindow() and SetLayeredWindowAttributes()
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
#include <vector>
#include <cmath>

#define ID_SHOW         2000
#define ID_EXIT         2001
#define ID_SEPARATOR    2002

#define ID_OPACITY_50   2003
#define ID_OPACITY_60   2004
#define ID_OPACITY_70   2005
#define ID_OPACITY_80   2006
#define ID_OPACITY_90   2007

#define ID_FULL_SCR     1000 // ID for each monitor goes like this: 1st = 100, 2nd = 101,...

POINT cursor; // Cursor position
BOOL is_hidden = false;
HINSTANCE instance;
HMENU menu_instance;
INT selected_monitor = 0;
INT checked_item = GetPrivateProfileInt(_T("Settings"), _T("checked_item"), ID_OPACITY_80, _T("./config.ini"));

std::vector <HMONITOR> monitorVec;

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    std::vector<HMONITOR>&monitorVec = *reinterpret_cast<std::vector<HMONITOR>*>(dwData);
    monitorVec.push_back(hMonitor);

    return TRUE;
}


BOOL ShowPopupMenu(HWND hWnd, POINT *curpos, int wDefaultItem)
{
    HMENU hPop = CreatePopupMenu();
    HMENU hSub = CreatePopupMenu();

    menu_instance = hPop;

    InsertMenu(hPop, 0, MF_BYPOSITION | MF_STRING, ID_SHOW, _T("Show/Hide"));
    InsertMenu(hPop, 1, MF_BYPOSITION | MF_SEPARATOR, ID_SEPARATOR, NULL);

    for (int i = 1; i < 6; i++)
    {
        std::stringstream ss;
        ss << (i+4)*10 << " %";
        std::string percentage = ss.str();

        if (checked_item == (2002 + i))
        {
            InsertMenu(hPop, (i + 1), MF_BYPOSITION | MF_CHECKED, (2002 + i), percentage.c_str());
        }
        else
        {
            InsertMenu(hPop, (i + 1), MF_BYPOSITION | MF_UNCHECKED, (2002 + i), percentage.c_str());
        }
    }

    if (monitorVec.size() > 1)
    {
        InsertMenu(hPop, 7, MF_BYPOSITION | MF_SEPARATOR, ID_SEPARATOR, NULL);
        InsertMenu(hPop, 8, MF_BYPOSITION | MF_POPUP, UINT(hSub), _T("Show on:"));

        InsertMenu(hSub, 0, MF_BYPOSITION | MF_STRING, ID_FULL_SCR, _T("All Monitors"));
        InsertMenu(hSub, 1, MF_BYPOSITION | MF_SEPARATOR, ID_SEPARATOR, NULL);

        for (std::vector <HMONITOR>::iterator it=monitorVec.begin(); it != monitorVec.end(); ++it)
        {
            INT i = std::distance(monitorVec.begin(), it);

            std::stringstream ss;
            ss << "Monitor " << i+1;
            std::string monitor_num = ss.str();

            if (selected_monitor == (100+i)) 
            {
                InsertMenu(hSub, (i+2), MF_BYPOSITION | MF_CHECKED, (100+i), monitor_num.c_str());
            }
            else
            {
                InsertMenu(hSub, (i+2), MF_BYPOSITION | MF_UNCHECKED, (100+i), monitor_num.c_str());
            }
        }
    }

    InsertMenu(hPop, 9, MF_BYPOSITION | MF_SEPARATOR, ID_SEPARATOR, NULL);
    InsertMenu(hPop, 10, MF_BYPOSITION | MF_STRING, ID_EXIT, _T("Exit"));

    SetMenuDefaultItem(hPop, ID_SHOW, FALSE);
    SetMenuDefaultItem(hSub, ID_FULL_SCR, FALSE);

    SetForegroundWindow(hWnd);
    SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);

    GetCursorPos(&cursor);
    WORD cmd = TrackPopupMenu(hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor.x, cursor.y, 0, hWnd, NULL);

    SendMessage(hWnd, WM_COMMAND, cmd, 0);

    DestroyMenu(hPop);
    return 0;
}

void RemoveTrayIcon(HWND hWnd, UINT uID)
{
    NOTIFYICONDATA nid;
    nid.hWnd = hWnd;
    nid.uID = uID;
    Shell_NotifyIcon(NIM_DELETE, &nid);
    DestroyIcon(nid.hIcon);
}

void AddTrayIcon(HWND hWnd, UINT uID, UINT uIcon)
{
    NOTIFYICONDATA nid;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = uID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_SHELLICON;

    nid.hIcon = LoadIcon(instance, _T("MAINICON"));
    _tcscpy(nid.szTip, _T("The Amazing Overlay App"));

    Shell_NotifyIcon(NIM_ADD, &nid);
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("MyWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;        // This is the handle for our window
    MSG messages;     // Here messages to the application are saved
    WNDCLASSEX wincl; // Data structure for the windowclass
    POINT cursor;     // Cursor position
    HANDLE file = CreateFile(_T("./config.ini"), GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

    if (file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(file);
    }
    else if (GetLastError() != ERROR_FILE_EXISTS)
    {
        // file exists
    }

    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&monitorVec); /* Get monitors count */
    GetCursorPos(&cursor);
    instance = hThisInstance;

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure; /* This function is called by windows */
    wincl.style = CS_DBLCLKS;            /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    // ExtractIconEx( "./icons/icon_256.ico", 0, NULL, &(wincl.hIcon), 1 );
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH)COLOR_WINDOWTEXT;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
        WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, /* Extended possibilites for variation */
        szClassName,                                                                             /* Classname */
        _T("The Amazing Overlay App"),                                                           /* Title Text */
        WS_POPUP | WS_VISIBLE,                                                                   /* default window */
        cursor.x, cursor.y, 1, 1, /*The position on the screen to create the window and window size*/
        HWND_DESKTOP,             /* The window is a child-window to desktop */
        NULL,                     /* No menu */
        hThisInstance,            /* Program Instance handler */
        NULL                      /* No Window Creation data */
    );

    switch (checked_item)
    {
    case ID_OPACITY_50:
        SetLayeredWindowAttributes(hwnd, 0, 127, LWA_ALPHA);
        checked_item = ID_OPACITY_50;
        WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2003"), _T("./config.ini"));
        break;
    case ID_OPACITY_60:
        SetLayeredWindowAttributes(hwnd, 0, 153, LWA_ALPHA);
        checked_item = ID_OPACITY_60;
        WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2004"), _T("./config.ini"));
        break;
    case ID_OPACITY_70:
        SetLayeredWindowAttributes(hwnd, 0, 178, LWA_ALPHA);
        checked_item = ID_OPACITY_70;
        WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2005"), _T("./config.ini"));
        break;
    case ID_OPACITY_80:
        SetLayeredWindowAttributes(hwnd, 0, 204, LWA_ALPHA);
        checked_item = ID_OPACITY_80;
        WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2006"), _T("./config.ini"));
        break;
    case ID_OPACITY_90:
        SetLayeredWindowAttributes(hwnd, 0, 230, LWA_ALPHA);
        checked_item = ID_OPACITY_90;
        WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2007"), _T("./config.ini"));
        break;
    default:
        break;
    }

    /* Make the window visible on the screen */
    ShowWindow(hwnd, SW_MAXIMIZE);

    /* Set the selected monitor (the monitor in which the window is on) */
    for (std::size_t i = 0, max = monitorVec.size(); i != max; ++i)
    {
        if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) == monitorVec[i]) 
        {
            selected_monitor = (100+i);
            break;
        }
    }

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
    {
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

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MONITORINFO mi; mi.cbSize = sizeof ( MONITORINFO );

    switch (message)
    { /* handle the messages */
    case WM_CREATE:
        AddTrayIcon(hwnd, 1, 0);
        break;
    case WM_QUIT:
    case WM_CLOSE:
        RemoveTrayIcon(hwnd, 1);
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0); /* send a WM_QUIT to the message queue */
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_SHOW:
            if (is_hidden)
            {
                ShowWindow(hwnd, SW_SHOW);
                is_hidden = false;
            }
            else
            {
                ShowWindow(hwnd, SW_HIDE);
                is_hidden = true;
            }
            break;
        case ID_OPACITY_50:
            SetLayeredWindowAttributes(hwnd, 0, 127, LWA_ALPHA);
            checked_item = ID_OPACITY_50;
            WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2003"), _T("./config.ini"));
            break;
        case ID_OPACITY_60:
            SetLayeredWindowAttributes(hwnd, 0, 153, LWA_ALPHA);
            checked_item = ID_OPACITY_60;
            WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2004"), _T("./config.ini"));
            break;
        case ID_OPACITY_70:
            SetLayeredWindowAttributes(hwnd, 0, 178, LWA_ALPHA);
            checked_item = ID_OPACITY_70;
            WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2005"), _T("./config.ini"));
            break;
        case ID_OPACITY_80:
            SetLayeredWindowAttributes(hwnd, 0, 204, LWA_ALPHA);
            checked_item = ID_OPACITY_80;
            WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2006"), _T("./config.ini"));
            break;
        case ID_OPACITY_90:
            SetLayeredWindowAttributes(hwnd, 0, 230, LWA_ALPHA);
            checked_item = ID_OPACITY_90;
            WritePrivateProfileString(_T("Settings"), _T("checked_item"), _T("2007"), _T("./config.ini"));
            break;
        case ID_FULL_SCR: 
            SetWindowPos(hwnd, NULL, GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN), GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN), SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            selected_monitor = ID_FULL_SCR;
            break;
        case ID_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        default:
            for (std::size_t i = 0, max = monitorVec.size(); i != max; ++i)
            {
                if (LOWORD(wParam) == (100+i)) 
                {
                    GetMonitorInfo(monitorVec[i], &mi);
                    int width = std::abs(mi.rcMonitor.right - mi.rcMonitor.left);
                    int height = std::abs(mi.rcMonitor.bottom - mi.rcMonitor.top);

                    SetWindowPos(hwnd, NULL, mi.rcMonitor.left, mi.rcMonitor.top, width, height, SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
                    selected_monitor = LOWORD(wParam);
                    break;
                }
            }
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        break;
    case WM_SHELLICON:
        switch (lParam)
        {
        case WM_RBUTTONUP:
            ShowPopupMenu(hwnd, NULL, -1);
            PostMessage(hwnd, WM_SHELLICON + 1, 0, 0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        break;
    default: /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
