#pragma once

#include "BasePatch.h"

template <typename T>
class MemoryPatch : public BasePatch
{
public:
	MemoryPatch(const char* file, int line, void* destination_address, T data, std::initializer_list<const char*> tags) :
		BasePatch(file, line, destination_address, tags), data(data)
	{
	}

	MemoryPatch(const char* file, int line, UINT32 destination_address, T data, std::initializer_list<const char*> tags) :
		MemoryPatch(file, line, (void*)destination_address, data, tags)
	{
	}

	size_t length()
	{
		return sizeof(T);
	}

	void do_apply()
	{
		memcpy(destination_address, &data, sizeof(T));
	}

private:
	T data;
};
