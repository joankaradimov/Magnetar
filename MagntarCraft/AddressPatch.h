#pragma once

#include <vector>

class PatchAddress
{
public:
	PatchAddress(void* destination_function, void* new_function);
	static void apply_patches();
private:
	static std::vector<std::pair<void*, void*>> patches;
};
