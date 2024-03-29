#include <Windows.h>
#include <vector>

#include "patching/FailStubPatch.h"

FailStubPatch::FailStubPatch(const char* file, int line, void* destination_function, std::initializer_list<const char*> tags) :
	BasePatch(file, line, destination_function, tags)
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
