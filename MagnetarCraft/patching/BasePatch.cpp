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
	for (BasePatch* patch : patches())
	{
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
