#pragma once

#include <initializer_list>
#include <vector>
#include <Windows.h>

class BasePatch {
public:
	BasePatch(const char* file, int line, void* destination_address, std::initializer_list<const char*> tags);

	bool is_pending();
	virtual size_t length() = 0;
	void apply();
	virtual void do_apply() = 0;

	static void apply_pending_patches();

protected:
	bool pending;
	BYTE* destination_address;
	const char* file;
	int line;
	std::vector<const char*> tags;

private:
	static std::vector<BasePatch*>& patches();
};
