#include "window.h"
#include "dv2.h"
#include "player.h"

#include "window.cpp"
#include "dv2.cpp"
#include "player.cpp"
#include "instrument.cpp"
#include "wav.cpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
			throw std::runtime_error("Failed to initialise COM");
		try
		{
			Window w(L"Wavmu", 720, 480, true);

			Player p;

			while (w.exists())
			{
				for (auto wke = w.keyboard.getEvent(); wke.type != WKET_INVALID; wke = w.keyboard.getEvent())
				{
					if (wke.type == WKET_KEYDOWN && wke.key == VK_SPACE)
						p.start();
					else if (wke.type == WKET_KEYUP && wke.key == VK_SPACE)
						p.stop();
				}

				// w.dv2.clear();

				// w.dv2.presentNoSync();
				w.update();
			}
		}
		catch (...)
		{
			CoUninitialize();
			throw;
		}
		CoUninitialize();
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