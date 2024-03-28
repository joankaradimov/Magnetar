#include "patching.h"
#include "warcraft2.h"

namespace game::warcraft2
{

void __stdcall init_current_thread_()
{
    dword_4D54E8 = GetCurrentThreadId();
}

FAIL_STUB_PATCH(init_current_thread, "warcraft2");

int __stdcall WinMain_(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
    hModule = hInstance;
    init_current_thread_();
    sub_45E760(hInstance, "War2Class");

    WNDCLASSA WndClass;
    memset(&WndClass, 0, sizeof(WndClass));

    WndClass.style = 8;
    WndClass.lpfnWndProc = (WNDPROC)sub_45EEB0;
    WndClass.hInstance = hModule;
    WndClass.hIcon = LoadIconA(hModule, (LPCSTR)0x65);
    WndClass.hCursor = LoadCursorA(0, (LPCSTR)0x7F00);
    WndClass.lpszClassName = "War2Class";
    WndClass.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

    if (!RegisterClassA(&WndClass))
    {
        sub_488A20("RegisterClass");
    }

    hWndParent = CreateWindowExA(
        WS_EX_TOPMOST,
        "War2Class",
        "Warcraft II",
        WS_SYSMENU | WS_VISIBLE | WS_POPUP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        0,
        0,
        hModule,
        0
    );
    if (!hWndParent)
    {
        sub_488A20("CreateWindowEx");
    }
    UpdateWindow(hWndParent);
    SetFocus(hWndParent);
    SetCursor(0);

    DWORD tick_count = GetTickCount();
    do
    {
        if (dword_4CE7E8 && hWndParent == GetForegroundWindow())
        {
            break;
        }
        sub_462B50();
        tagMSG WndClass;
        while (PeekMessageA(&WndClass, 0, 0, 0, 1))
        {
            if (hAccTable && hWndParent && TranslateAcceleratorA(hWndParent, hAccTable, &WndClass))
            {
                if (WndClass.hwnd == hWndParent && (WndClass.message == 256 || WndClass.message == 260))
                {
                    sub_48A7B0(WndClass.wParam);
                }
            }
            else
            {
                TranslateMessage(&WndClass);
                DispatchMessageA(&WndClass);
            }
        }
    } while (GetTickCount() <= tick_count + 2000);

    GameMainLoop(TranslateMessage, TranslateAcceleratorA, PeekMessageA);
    return 0;
}

FAIL_STUB_PATCH(_WinMain, "warcraft2");

}
