#pragma once

#include "BasePatch.h"

class FailStubPatch : public BasePatch
{
public:
	FailStubPatch(const char* file, int line, void* destination_function, std::initializer_list<const char*> tags);

	size_t length();
	void do_apply();

private:
	static void fail() {
		exit(1);
	}

	static const BYTE JMP_INSTRUCTION_OPCODE = 0xE9;
	static const BYTE INT3_INSTRUCTION_OPCODE = 0xCC;
};
