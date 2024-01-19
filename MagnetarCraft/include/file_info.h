#include <filesystem>
#include <Windows.h>

class FileInfo
{
public:
	FileInfo(const std::filesystem::path& path);
	~FileInfo();

	bool is_valid();
	VS_FIXEDFILEINFO* operator->();

private:
	void* version_data;
	VS_FIXEDFILEINFO* file_info;
};
