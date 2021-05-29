#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "wnd_proc.h"
#include "db.h"

static BOOL OnCreate(HWND, LPCREATESTRUCT);                                     	// WM_CREATE
static void OnCommand(HWND, int, HWND, UINT);                                      	// WM_COMMAND
static void OnPaint(HWND);                                                        	// WM_PAINT
static void OnDestroy(HWND);                                                        // WM_DESTROY

#define IDC_GROUP1 1001 // Група 1.
#define IDC_LANG_LIST 1002 // Список мов.
#define IDC_REQUEST 1003 // Рядок запиту для перекладу.
#define IDC_RESULT 1004 // Результат.
#define IDC_TRANSLATE 1005 // Кнопка "Перекласти".

#define IDC_GROUP2 1006 // Група 2.
#define IDC_UKRAINIAN_WORD 1007 // Слово на українській.
#define IDC_ENGLISH_WORD 1008 // Слово на англійській.
#define IDC_ADD_WORDS 1009 // Кнопка "Додавити".

static HFONT g_hDefaultFont = NULL; // Шрифт.

// Мови.
enum LANGUAGES {
    LANGUAGE_ENGLISH = 0,
    LANGUAGE_UKRAINIAN = 1
};

static enum LANGUAGES g_lang = LANGUAGE_ENGLISH; // Поточна мова, з якої буде вестись переклад. ENGLISH = Default.

// [WindowProcedure]: 
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

    case WM_ERASEBKGND:
        return 1;
    
    case WM_CTLCOLORSTATIC:
    {
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)(HBRUSH)GetStockObject(LTGRAY_BRUSH);
    }
    break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
// [/WindowProcedure]


// [OnCreate]: WM_CREATE
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
    InitCommonControls();
    
    RECT rc;
    GetClientRect(hWnd, &rc);
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;
    
    // Створення шрифта.
    g_hDefaultFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
    
    // Додавання усіх контролів в головне вікно.
    
    CreateWindowEx(0, "button", "Перекладач", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, 10, 320, 190, hWnd, (HMENU)IDC_GROUP1, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_GROUP1), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    
    HWND hLangList = CreateWindowEx(0, "combobox", "",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT | CBS_HASSTRINGS,
        20, 30, 300, 100, hWnd, (HMENU)IDC_LANG_LIST, lpcs->hInstance, NULL);
    SendMessage(hLangList, WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    ComboBox_AddString(hLangList, "Переклад з англійської на українську мову");
    ComboBox_AddString(hLangList, "Переклад з української на англійську мову");
    ComboBox_SetCurSel(hLangList, g_lang);
    
    CreateWindowEx(0, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        20, 90, 300, 20, hWnd, (HMENU)IDC_REQUEST, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_REQUEST), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_REQUEST), 128);
    
    CreateWindowEx(0, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        20, 140, 300, 20, hWnd, (HMENU)IDC_RESULT, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_RESULT), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_RESULT), 128);
    
    CreateWindowEx(0, "button", "Перекласти", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, 170, 100, 20, hWnd, (HMENU)IDC_TRANSLATE, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_TRANSLATE), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    
    CreateWindowEx(0, "button", "Додавання слів", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        340, 10, 320, 190, hWnd, (HMENU)IDC_GROUP2, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_GROUP2), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    
    CreateWindowEx(0, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        350, 50, 300, 20, hWnd, (HMENU)IDC_UKRAINIAN_WORD, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_UKRAINIAN_WORD), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_UKRAINIAN_WORD), 64);
    
    CreateWindowEx(0, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        350, 100, 300, 20, hWnd, (HMENU)IDC_ENGLISH_WORD, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_ENGLISH_WORD), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_ENGLISH_WORD), 64);
    
    CreateWindowEx(0, "button", "Додати", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        450, 130, 100, 20, hWnd, (HMENU)IDC_ADD_WORDS, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_ADD_WORDS), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    
    // Назва бази даних с словником.
    DB_SetFileName("db.txt");
    
    return TRUE;
}
// [/OnCreate]


// [OnCommand]: WM_COMMAND
static void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    // При зміні мови.
    case IDC_LANG_LIST:
    {
        g_lang = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_LANG_LIST));
    }
    break;
    
    // При натисканні кнопки "Перекласти".
    case IDC_TRANSLATE:
    {
        char szRequest[128];
        memset(szRequest, 0, 128);
        if (GetWindowTextLength(GetDlgItem(hWnd, IDC_REQUEST)) > 0)
        {
            GetWindowText(GetDlgItem(hWnd, IDC_REQUEST), szRequest, 128);
            
            char szResult[256];
            memset(szResult, 0, 256);
            
            if (g_lang == LANGUAGE_ENGLISH)
            {
                DB_Translate(szRequest, szResult, ENGLISH_TO_UKRAINIAN);
            }
            else if (g_lang == LANGUAGE_UKRAINIAN)
            {
                DB_Translate(szRequest, szResult, UKRAINIAN_TO_ENGLISH);
            }
            
            SetWindowText(GetDlgItem(hWnd, IDC_RESULT), szResult);
        }
    }
    break;
    
    // При натисканні кноки "Додати".
    case IDC_ADD_WORDS:
    {
        char szUkrainian[64];
        memset(szUkrainian, 0, 64);
        if (GetWindowTextLength(GetDlgItem(hWnd, IDC_UKRAINIAN_WORD)) > 0)
        {
            GetWindowText(GetDlgItem(hWnd, IDC_UKRAINIAN_WORD), szUkrainian, 64);
        }
        else
        {
            MessageBox(hWnd, "Заповніть усі поля!", "Сповіщення", MB_OK | MB_ICONINFORMATION);
            break;
        }
        
        char szEnglish[64];
        memset(szEnglish, 0, 64);
        if (GetWindowTextLength(GetDlgItem(hWnd, IDC_ENGLISH_WORD)) > 0)
        {
            GetWindowText(GetDlgItem(hWnd, IDC_ENGLISH_WORD), szEnglish, 64);
        }
        else
        {
            MessageBox(hWnd, "Заповніть усі поля!", "Сповіщення", MB_OK | MB_ICONINFORMATION);
            break;
        }
        
        if (DB_AddWords(szUkrainian, szEnglish) == TRUE)
        {
            MessageBox(hWnd, "Додано!", "Сповіщення", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(hWnd, "Відбулася невідома помилка!", "Помилка", MB_OK | MB_ICONERROR);
        }
    }
    break;
    }
}
// [/OnCommand]


// [OnPaint]: WM_PAINT
static void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, iWindowWidth, iWindowHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    FillRect(hMemDC, &ps.rcPaint, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

    int iOldBkMode = SetBkMode(hMemDC, TRANSPARENT);
    HFONT hOldFont = (HFONT)GetCurrentObject(hMemDC, OBJ_FONT);
    HBRUSH hOldBrush = (HBRUSH)GetCurrentObject(hMemDC, OBJ_BRUSH);
    HPEN hOldPen = (HPEN)GetCurrentObject(hMemDC, OBJ_PEN);
    COLORREF clrOldColor = GetTextColor(hMemDC);

    SelectObject(hMemDC, g_hDefaultFont);
    
    char szText[1024];
    
    lstrcpy(szText, "Введіть слово для перекладу");
    TextOut(hMemDC, 20, 70, szText, lstrlen(szText));
    
    lstrcpy(szText, "Результат");
    TextOut(hMemDC, 20, 120, szText, lstrlen(szText));
    
    lstrcpy(szText, "Введіть слово українською мовою");
    TextOut(hMemDC, 350, 30, szText, lstrlen(szText));
    
    lstrcpy(szText, "Введіть слово англійською мовою");
    TextOut(hMemDC, 350, 80, szText, lstrlen(szText));

    SetTextColor(hMemDC, clrOldColor);
    SelectObject(hMemDC, hOldPen);
    SelectObject(hMemDC, hOldBrush);
    SelectObject(hMemDC, hOldFont);
    SetBkMode(hMemDC, iOldBkMode);

    BitBlt(hDC, 0, 0, iWindowWidth, iWindowHeight, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    EndPaint(hWnd, &ps);
}
// [/OnPaint]


// [OnDestroy]: WM_DESTROY
static void OnDestroy(HWND hWnd)
{
    DeleteObject(g_hDefaultFont);
    
    PostQuitMessage(0);
}
// [/OnDestroy]

