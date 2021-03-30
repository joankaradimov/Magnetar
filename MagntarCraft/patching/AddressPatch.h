#pragma once

#include "BasePatch.h"

class AddressPatch: public BasePatch
{
public:
	AddressPatch(void* destination_function, void* replacement_function);

	size_t length();
	void apply();

private:
	BYTE* replacement_function;

	static const BYTE JMP_INSTRUCTION_OPCODE = 0xE9;
};
