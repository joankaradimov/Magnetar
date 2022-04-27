#pragma once

#include "BasePatch.h"

class FunctionPatch: public BasePatch
{
public:
	FunctionPatch(const char* file, int line, void* destination_function, void* replacement_function);

	size_t length();
	void do_apply();

private:
	BYTE* replacement_function;

	static const BYTE JMP_INSTRUCTION_OPCODE = 0xE9;
};
