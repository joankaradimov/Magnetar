#include "NopPatch.h"

NopPatch::NopPatch(void* destination_address, size_t block_length) :
	BasePatch(destination_address),
	block_length(block_length)
{
}

size_t NopPatch::length()
{
	return block_length;
}

void NopPatch::apply()
{
	for (int i = 0; i < block_length; i++)
	{
		destination_address[i] = NOP_INSTRUCTION_OPCODE;
	}
}
