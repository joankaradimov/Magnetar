#include "patching.h"
#include "warcraft2.h"

namespace game::warcraft2
{

void __stdcall init_current_thread_()
{
    dword_4D54E8 = GetCurrentThreadId();
}

FUNCTION_PATCH(init_current_thread, init_current_thread_, "warcraft2");

}
