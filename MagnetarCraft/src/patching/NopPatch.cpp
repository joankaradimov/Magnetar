#include "patching/NopPatch.h"

NopPatch::NopPatch(const char* file, int line, void* destination_address, size_t block_length, std::initializer_list<const char*> tags) :
	BasePatch(file, line, destination_address, tags),
	block_length(block_length)
{
}

size_t NopPatch::length()
{
	return block_length;
}

void NopPatch::do_apply()
{
	for (int i = 0; i < block_length; i++)
	{
		destination_address[i] = NOP_INSTRUCTION_OPCODE;
	}
}
