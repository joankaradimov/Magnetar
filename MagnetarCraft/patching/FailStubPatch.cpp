#include <Windows.h>
#include <vector>

#include "FailStubPatch.h"

FailStubPatch::FailStubPatch(const char* file, int line, void* destination_function) :
	BasePatch(file, line, destination_function)
{
}

size_t FailStubPatch::length()
{
	return sizeof(INT3_INSTRUCTION_OPCODE) + sizeof(JMP_INSTRUCTION_OPCODE) + sizeof(&FailStubPatch::fail);
}

void FailStubPatch::do_apply()
{
	destination_address[0] = INT3_INSTRUCTION_OPCODE;

	ptrdiff_t jmp_offset = (BYTE*)FailStubPatch::fail - destination_address - length();
	destination_address[1] = JMP_INSTRUCTION_OPCODE;
	*((ptrdiff_t*)(destination_address + 2)) = jmp_offset;
}
