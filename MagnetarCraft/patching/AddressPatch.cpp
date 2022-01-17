#include <Windows.h>
#include <vector>

#include "AddressPatch.h"

AddressPatch::AddressPatch(void* destination_function, void* replacement_function) :
	BasePatch(destination_function),
	replacement_function((BYTE*)replacement_function)
{
}

size_t AddressPatch::length()
{
	return sizeof(JMP_INSTRUCTION_OPCODE) + sizeof(replacement_function);
}

void AddressPatch::do_apply()
{
	ptrdiff_t jmp_offset = replacement_function - destination_address - length();
	*destination_address = JMP_INSTRUCTION_OPCODE;
	*((ptrdiff_t*)(destination_address + 1)) = jmp_offset;
}
