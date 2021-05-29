#include <windows.h>

#include "wnd_proc.h"

// [WinMain]: точка входу в програму.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    const char szWindowName[] = "Translator";
    const char szClassName[] = "__Translator__class__";
    const char szMutexName[] = "__Translator__mutex__";
    
    // Створюємо мютекс. Для запуску лише одного екземпляру програми.
    HANDLE hMutex = CreateMutex(NULL, FALSE, szMutexName);
    if (hMutex == NULL)
    {
        MessageBox(NULL, "Could not create mutex.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1;
    }
    else
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            MessageBox(NULL, "An instance of the program is already running.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
            
            HWND hWnd = FindWindow(szClassName, szWindowName);
            if (hWnd)
            {
                if (IsIconic(hWnd))
                {
                    ShowWindow(hWnd, SW_RESTORE);
                }
                SetForegroundWindow(hWnd);
            }
            
            CloseHandle(hMutex);
            return 1;
        }
    }
    
    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
    DWORD dwExStyle = WS_EX_APPWINDOW;
    
    RECT rc;
    SetRect(&rc, 0, 0, 670, 210);
    AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);
    
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;
    
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    // Реєстрація класу вікна.
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, "Window Registration Failed.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 1;
    }
    
    // Створення вікна.
    HWND hWnd = CreateWindowEx(
        dwExStyle,
        szClassName,
        szWindowName,
        dwStyle,
        (GetSystemMetrics(SM_CXSCREEN) >> 1) - (iWindowWidth >> 1),
        ((rc.bottom - rc.top) >> 1) - (iWindowHeight >> 1),
        iWindowWidth,
        iWindowHeight,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL);
    
    if (!hWnd)
    {
        MessageBox(NULL, "Window Creation Failed.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
        UnregisterClass(szClassName, hInstance);
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 1;
    }
    
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
    
    // Опрацювання подій.
    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            DispatchMessage(&msg);
            TranslateMessage(&msg);
        }
    }
    
    UnregisterClass(szClassName, hInstance);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return (int)msg.wParam;
}
// [/WinMain]

