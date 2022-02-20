#pragma once

#include "BasePatch.h"

template <typename T>
class MemoryPatch : public BasePatch
{
public:
	MemoryPatch(void* destination_address, T data) : BasePatch(destination_address), data(data)
	{
	}

	MemoryPatch(UINT32 destination_address, T data) : MemoryPatch((void*)destination_address, data)
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
