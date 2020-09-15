#include "window.h"
#include "dv2.h"
#include "player.h"

#include "window.cpp"
#include "dv2.cpp"
#include "player.cpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		Window w(L"Wavmu", 720, 480, true);

		while (w.exists())
		{
			w.dv2.clear();

			w.dv2.presentNoSync();
			w.updateBlocking();
		}
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Fatal error", MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxW(nullptr, L"Unknown error.", L"Fatal error", MB_ICONERROR);
	}

	return 0;
}