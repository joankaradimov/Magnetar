#include <exception>
#include <Windows.h>

#include "MagnetarCraft.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) try
{
	StartMagnetar();

	return 0;
}
catch (const std::exception& e) {
	report_error(e.what());
	return 1;
}
