#include "file_info.h"

FileInfo::FileInfo(const std::filesystem::path& path) : version_data(nullptr)
{
	DWORD _unused;
	int version_info_size = GetFileVersionInfoSize(path.c_str(), &_unused);
	if (version_info_size)
	{
		version_data = malloc(version_info_size);
		if (GetFileVersionInfo(path.c_str(), 0, version_info_size, version_data))
		{
			VS_FIXEDFILEINFO* file_info_buffer;
			unsigned int file_info_buffer_length;
			if (VerQueryValueA(version_data, "\\", (LPVOID*)&file_info_buffer, &file_info_buffer_length))
			{
				file_info = file_info_buffer;
			}
		}
	}
}

FileInfo::~FileInfo()
{
	if (version_data)
	{
		free(version_data);
	}
}

bool FileInfo::is_valid()
{
	return version_data != nullptr;
}

VS_FIXEDFILEINFO* FileInfo::operator->()
{
	return file_info;
}
