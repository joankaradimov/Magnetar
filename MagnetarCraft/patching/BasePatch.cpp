#include <algorithm>
#include <Windows.h>

#include "BasePatch.h"

BasePatch::BasePatch(void* destination_address): destination_address((BYTE*)destination_address), pending(true)
{
	patches().push_back(this);
}

bool BasePatch::is_pending() {
	return pending;
}

void BasePatch::apply() {
	pending = false;

	do_apply();
}

void BasePatch::apply_pending_patches() {
	// TODO: batch calls to `VirtualProtect` together
	BasePatch* previous_patch = nullptr;

	std::sort(patches().begin(), patches().end(), [](auto a, auto b) {return a->destination_address < b->destination_address; });
	for (BasePatch* patch : patches())
	{
		if (previous_patch && patch->destination_address <= previous_patch->destination_address + previous_patch->length())
		{
			throw std::exception("Two patches overlap and are in conflict");
		}
		else
		{
			previous_patch = patch;
		}

		if (patch->is_pending())
		{
			DWORD old_protection;
			VirtualProtect(patch->destination_address, patch->length(), PAGE_EXECUTE_READWRITE, &old_protection);

			patch->apply();

			DWORD unused;
			VirtualProtect(patch->destination_address, patch->length(), old_protection, &unused);
		}
	}
}

std::vector<BasePatch*>& BasePatch::patches() {
	// Initializing the `patches` array this way ensures that its constructor
	// is called before the constructors of each of the individual patches
	static std::vector<BasePatch*> patches;
	return patches;
}
