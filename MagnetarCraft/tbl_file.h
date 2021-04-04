#pragma once

#include <string>

#include "starcraft.h"

class TblFile {
public:
	TblFile(const char* file_name) :
		count(0),
		offsets(nullptr),
		data(nullptr)
	{
		HANDLE hFile;
		if (!SFileOpenFileEx(0, file_name, 0, &hFile))
		{
			std::string message = std::string("Could not open '") + file_name + "'";
			throw std::exception(message.c_str());
		}

		int fileSize = SFileGetFileSize(hFile, 0);
		if (fileSize == -1)
		{
			FileFatal(hFile, GetLastError());
		}
		if (fileSize == 0)
		{
			SysWarn_FileNotFound(file_name, 24);
		}

		if (fileSize < 2) {
			return;
		}

		int bytesRead;
		SFileReadFile(hFile, &count, 2, &bytesRead, 0);
		if (bytesRead < 2)
		{
			std::string message = std::string("Error reading string count from '") + file_name + "'";
			throw std::exception(message.c_str());
		}

		offsets = new unsigned __int16[count];
		SFileReadFile(hFile, offsets, count * sizeof(*offsets), &bytesRead, 0);
		if (bytesRead < count * sizeof(*offsets))
		{
			std::string message = std::string("Error reading offsets from '") + file_name + "'";
			throw std::exception(message.c_str());
		}

		int data_size = fileSize - sizeof(count) - count * sizeof(*offsets);
		data = new char[data_size];
		SFileReadFile(hFile, data, data_size, &bytesRead, 0);
		if (bytesRead < data_size)
		{
			std::string message = std::string("Error reading string data from '") + file_name + "'";
			throw std::exception(message.c_str());
		}
	}

	const char* operator[](size_t index)
	{
		if (index < count)
		{
			return data + offsets[index] - offsets[0];
		}
		return "";
	}

	~TblFile()
	{
		if (offsets)
		{
			delete[] offsets;
		}

		if (data)
		{
			delete[] data;
		}
	}

private:
	unsigned __int16 count;
	unsigned __int16* offsets;
	char* data;
};
