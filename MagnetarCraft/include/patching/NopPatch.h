#pragma once

#include <initializer_list>

#include "BasePatch.h"

class NopPatch : public BasePatch
{
public:
	NopPatch(const char* file, int line, void* destination_address, size_t block_length, std::initializer_list<const char*> tags);

	size_t length();
	void do_apply();

private:
	size_t block_length;

	static const BYTE NOP_INSTRUCTION_OPCODE = 0x90;
};
