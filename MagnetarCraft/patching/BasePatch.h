#pragma once

#include <vector>
#include <Windows.h>

class BasePatch {
public:
	BasePatch(void* destination_address);

	bool is_pending();
	virtual size_t length() = 0;
	void apply();
	virtual void do_apply() = 0;

	static void apply_pending_patches();

protected:
	bool pending;
	BYTE* destination_address;

private:
	static std::vector<BasePatch*> patches;
};
