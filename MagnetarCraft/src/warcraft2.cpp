#include "patching.h"
#include "magnetorm.h"
#include "warcraft2.h"

namespace game::warcraft2
{

void __stdcall init_current_thread_()
{
    dword_4D54E8 = GetCurrentThreadId();
}

FAIL_STUB_PATCH(init_current_thread, "warcraft2");

void __cdecl sub_41C750_(int a1)
{
    struct_arg0 v3;

    sub_440D10();
    int v1 = sub_4875C0(&v3, a1);
    if (dword_4AE198)
    {
        dword_4AE198(0);
    }

    if (v1 && (byte_4AE0F9 || !sub_405610(&v3)))
    {
        auto v2 = funcs_41C7B5[v3.word0];
        if (v2)
        {
            v2(&v3);
        }
    }
}

FUNCTION_PATCH(sub_41C750, sub_41C750_, "warcraft2");

void sub_43B330_(int a1)
{
    HANDLE v2;
    char Buffer[256];

    sub_489670();
    sub_4051D0();
    sub_48A3C0();
    byte_4B50E5 = 1;
    nullsub_1();
    sprintf(Buffer, "Smk\\%s", off_4A143C[0]);
    sub_41C6E0();
    sub_487A00();
    dword_4AE19C = sub_43B280;
    dword_4AE168 = sub_43B250;
    dword_4AE17C = sub_43B250;
    dword_4AE188 = sub_43B250;
    dword_4AE164 = sub_43B2E0;
    nullsub_1();
    byte_4B50E4 = 0;
    SVidPlayBegin(Buffer, 0, 0, 0, 0, 0x10280808, &v2);
    if (v2)
    {
        while (!byte_4B50E4)
        {
            if (dword_4CE7E8 && !SVidPlayContinueSingle(v2, 0, 0))
            {
                break;
            }
            sub_41C750_(3);
            Sleep(0);
        }
        SVidPlayEnd(v2);
    }
    nullsub_1();
    sub_437030(a1);
    sub_441250();
    if (sub_428460())
    {
        if (dword_4D6B50 & 0x200)
        {
            dword_4D6B50 &= ~0x200;
            if (get_game_mode() == GAME_LOGO)
            {
                byte_4B50E5 = 1;
                nullsub_1();
                sprintf(Buffer, "Smk\\%s", off_4A143C[1]);
                sub_41C6E0();
                sub_487A00();
                dword_4AE168 = sub_43B250;
                dword_4AE17C = sub_43B250;
                dword_4AE188 = sub_43B250;
                dword_4AE19C = sub_43B280;
                dword_4AE164 = sub_43B2E0;
                nullsub_1();
                byte_4B50E4 = 0;
                SVidPlayBegin(Buffer, 0, 0, 0, 0, 271058952, &v2);
                if (v2)
                {
                    while (!byte_4B50E4)
                    {
                        if (dword_4CE7E8 && !SVidPlayContinueSingle(v2, 0, 0))
                        {
                            break;
                        }
                        sub_41C750(3);
                        Sleep(0);
                    }
                    SVidPlayEnd(v2);
                }
                nullsub_1();
                sub_437030(a1);
                sub_441250();
            }
        }
    }
    sub_48A3C0();
}

void __cdecl sub_43B330__()
{
    int a1;

    __asm mov a1, ebx

    sub_43B330_(a1);
}

FUNCTION_PATCH((void*)0x43B330, sub_43B330__, "warcraft2");

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
