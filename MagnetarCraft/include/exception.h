#pragma once

#include <exception>

class FileNotFoundException : public std::exception
{
public:
	FileNotFoundException(const char* const message, int error_code)
		: std::exception(message), error_code(error_code)
	{
	}

	const int error_code;
};

// TODO: figure out where to put this definition
void SysWarn_FileNotFound_(const char* a1, int last_error);
