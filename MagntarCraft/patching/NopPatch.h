#pragma once

#include <vector>

class NopPatch
{
public:
	NopPatch(void* destination_address, size_t length);
	static void apply_patches();
private:
	static std::vector<std::pair<void*, size_t>> patches;
};
