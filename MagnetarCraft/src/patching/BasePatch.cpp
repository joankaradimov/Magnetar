#include <algorithm>
#include <sstream>
#include <Windows.h>

#include "patching/BasePatch.h"

BasePatch::BasePatch(const char* file, int line, void* destination_address):
	file(file),
	line(line),
	destination_address((BYTE*)destination_address),
	pending(true)
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
		if (previous_patch && patch->destination_address < previous_patch->destination_address + previous_patch->length())
		{
			std::ostringstream error_message;
			error_message << "Two patches overlap and are in conflict:" << std::endl;
			error_message << '\t' << patch->file << ':' << patch->line << std::endl;
			error_message << '\t' << previous_patch->file << ':' << previous_patch->line << std::endl;
			throw std::exception(error_message.str().c_str());
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
