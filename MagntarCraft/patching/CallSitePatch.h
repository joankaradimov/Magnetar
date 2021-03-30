#pragma once

#include <vector>

class CallSitePatch
{
public:
	CallSitePatch(void* address, void* new_function);
	static void apply_patches();
private:
	static std::vector<std::pair<void*, void*>> patches;
};
