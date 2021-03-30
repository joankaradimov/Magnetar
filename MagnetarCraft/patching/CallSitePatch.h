#pragma once

#include "BasePatch.h"

class CallSitePatch : public BasePatch
{
public:
	CallSitePatch(void* destination_address, void* function);

	size_t length();
	void apply();

private:
	BYTE* function;

	static const BYTE CALL_INSTRUCTION_OPCODE = 0xE8;
};
