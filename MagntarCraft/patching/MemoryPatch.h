#pragma once

#include "BasePatch.h"

class MemoryPatch : public BasePatch
{
public:
	MemoryPatch(UINT32 destination_address, void* block, size_t block_length);
	MemoryPatch(void* destination_address, void* block, size_t block_length);

	size_t length();
	void apply();

private:
	BYTE* block;
	size_t block_length;
};
