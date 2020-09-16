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
		if (FAILED(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED)))
			throw std::runtime_error("Failed to initialise COM and Windows Runtime APIs");
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
		catch (...)
		{
			Windows::Foundation::Uninitialize();
			throw;
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