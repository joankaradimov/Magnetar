#pragma once

#include <vector>

class AddressPatch
{
public:
	AddressPatch(void* destination_function, void* new_function);
	static void apply_patches();
private:
	static std::vector<std::pair<void*, void*>> patches;
};
