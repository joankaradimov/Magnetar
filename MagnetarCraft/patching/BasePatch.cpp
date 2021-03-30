#include <Windows.h>

#include "BasePatch.h"

std::vector<BasePatch*> BasePatch::pending_patches;

BasePatch::BasePatch(void* destination_address): destination_address((BYTE*)destination_address)
{
	pending_patches.push_back(this);
}

void BasePatch::apply_pending_patches() {
	// TODO: batch calls to `VirtualProtect` together
	for (BasePatch* patch : pending_patches)
	{
		DWORD old_protection;
		VirtualProtect(patch->destination_address, patch->length(), PAGE_EXECUTE_READWRITE, &old_protection);

		patch->apply();

		DWORD unused;
		VirtualProtect(patch->destination_address, patch->length(), old_protection, &unused);
	}
}
