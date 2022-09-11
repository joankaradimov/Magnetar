#include <Windows.h>
#include <vector>

#include "patching/CallSitePatch.h"

CallSitePatch::CallSitePatch(const char* file, int line, void* destination_address, void* function) :
	BasePatch(file, line, destination_address),
	function((BYTE*)function)
{
}

size_t CallSitePatch::length()
{
	return sizeof(CALL_INSTRUCTION_OPCODE) + sizeof(function);
}

void CallSitePatch::do_apply()
{
	ptrdiff_t jmp_offset = function - destination_address - length();
	*destination_address = CALL_INSTRUCTION_OPCODE;
	*((ptrdiff_t*)(destination_address + 1)) = jmp_offset;
}
