#pragma once

#include "BasePatch.h"

class FailStubPatch : public BasePatch
{
public:
	FailStubPatch(void* destination_function);

	size_t length();
	void do_apply();

private:
	static void fail() {
		exit(1);
	}

	static const BYTE JMP_INSTRUCTION_OPCODE = 0xE9;
	static const BYTE INT3_INSTRUCTION_OPCODE = 0xCC;
};
