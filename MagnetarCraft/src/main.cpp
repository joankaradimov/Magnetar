#include <exception>
#include <Windows.h>

#include "MagnetarCraft.h"
#include "exception.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) try
{
	StartMagnetar();

	return 0;
}
catch (const FileNotFoundException& e)
{
	SysWarn_FileNotFound_(e.what(), e.error_code);
	return 1;
}
catch (const std::exception& e) {
	report_error(e.what());
	return 1;
}
