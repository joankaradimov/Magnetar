#include <Windows.h>
#include <vector>

#include "NopPatch.h"

const BYTE NOP_OPCODE = 0x90;

std::vector<std::pair<void*, size_t>> NopPatch::patches;

NopPatch::NopPatch(void* destination_address, size_t length)
{
	patches.emplace_back(destination_address, length);
}

void NopPatch::apply_patches()
{
	// TODO: batch calls to `VirtualProtect` together
	for (std::pair<void*, size_t>& patch : patches)
	{
		BYTE* destination_address = (BYTE*)patch.first;
		size_t length = patch.second;

		DWORD old_protection;
		VirtualProtect(destination_address, length, PAGE_EXECUTE_READWRITE, &old_protection);

		for (int i = 0; i < length; i++)
		{
			destination_address[i] = NOP_OPCODE;
		}

		VirtualProtect(destination_address, length, old_protection, NULL);
	}
}
