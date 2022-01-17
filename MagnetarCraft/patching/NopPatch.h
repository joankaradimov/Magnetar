#pragma once

#include "BasePatch.h"

class NopPatch : public BasePatch
{
public:
	NopPatch(void* destination_address, size_t block_length);

	size_t length();
	void do_apply();

private:
	size_t block_length;

	static const BYTE NOP_INSTRUCTION_OPCODE = 0x90;
};
