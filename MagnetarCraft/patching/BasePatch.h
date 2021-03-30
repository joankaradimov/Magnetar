#pragma once

#include <vector>
#include <Windows.h>

class BasePatch {
public:
	BasePatch(void* destination_address);

	virtual size_t length() = 0;
	virtual void apply() = 0;

	static void apply_pending_patches();

protected:
	BYTE* destination_address;

private:
	static std::vector<BasePatch*> pending_patches;
};
