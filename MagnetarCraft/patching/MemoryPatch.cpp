#include "MemoryPatch.h"

MemoryPatch::MemoryPatch(void* destination_address, void* block, size_t block_length) :
	BasePatch(destination_address),
	block((BYTE*)block),
	block_length(block_length)
{
}

MemoryPatch::MemoryPatch(UINT32 destination_address, void* block, size_t block_length) :
	MemoryPatch((void*)destination_address, block, block_length)
{
}

size_t MemoryPatch::length()
{
	return block_length;
}

void MemoryPatch::apply()
{
	memcpy(destination_address, &block, block_length);
}
