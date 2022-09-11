#include <Windows.h>
#include <vector>

#include "patching/FunctionPatch.h"

FunctionPatch::FunctionPatch(const char* file, int line, void* destination_function, void* replacement_function) :
	BasePatch(file, line, destination_function),
	replacement_function((BYTE*)replacement_function)
{
}

size_t FunctionPatch::length()
{
	return sizeof(JMP_INSTRUCTION_OPCODE) + sizeof(replacement_function);
}

void FunctionPatch::do_apply()
{
	ptrdiff_t jmp_offset = replacement_function - destination_address - length();
	*destination_address = JMP_INSTRUCTION_OPCODE;
	*((ptrdiff_t*)(destination_address + 1)) = jmp_offset;
}
