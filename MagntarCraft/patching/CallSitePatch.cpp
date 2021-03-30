#include <Windows.h>
#include <vector>

#include "CallSitePatch.h"

const BYTE JMP_INSTRUCTION_OPCODE = 0xE8;
const ptrdiff_t JMP_INSTRUCTION_LENGTH = 5;

std::vector<std::pair<void*, void*>> CallSitePatch::patches;

CallSitePatch::CallSitePatch(void* destination_function, void* new_function)
{
	patches.emplace_back(destination_function, new_function);
}

void CallSitePatch::apply_patches()
{
	// TODO: batch calls to `VirtualProtect` together
	for (std::pair<void*, void*>& patch : patches)
	{
		BYTE* destination_function = (BYTE*)patch.first;
		BYTE* new_function = (BYTE*)patch.second;

		DWORD old_protection;
		VirtualProtect(destination_function, JMP_INSTRUCTION_LENGTH, PAGE_EXECUTE_READWRITE, &old_protection);

		ptrdiff_t jmp_offset = new_function - destination_function - JMP_INSTRUCTION_LENGTH;
		destination_function[0] = JMP_INSTRUCTION_OPCODE;
		*((ptrdiff_t*)(destination_function + 1)) = jmp_offset;

		VirtualProtect(destination_function, JMP_INSTRUCTION_LENGTH, old_protection, NULL);
	}
}
