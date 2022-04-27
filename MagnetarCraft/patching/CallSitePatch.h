#pragma once

#include "BasePatch.h"

class CallSitePatch : public BasePatch
{
public:
	CallSitePatch(const char* file, int line, void* destination_address, void* function);

	size_t length();
	void do_apply();

private:
	BYTE* function;

	static const BYTE CALL_INSTRUCTION_OPCODE = 0xE8;
};
