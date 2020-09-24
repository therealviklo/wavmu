#include "window.h"
#include "dv2.h"
#include "player.h"

#include "window.cpp"
#include "dv2.cpp"
#include "player.cpp"
#include "instrument.cpp"
#include "wav.cpp"
#include "track.cpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
			throw std::runtime_error("Failed to initialise COM");
		try
		{
			Window w(L"Wavmu", 720, 480, true);

			Sections sections = {std::make_shared<Section>(Section{{
				{0.5, 0.5, 69},
				{0.0, 0.5, 60},
				{1.5, 0.5, 69},
				{1.0, 0.5, 60},
				{2.5, 0.5, 69},
				{2.0, 0.5, 60},
				{3.5, 0.5, 69},
				{3.0, 0.5, 60},
				{4.5, 0.5, 69},
				{4.0, 0.5, 60}
			}})};

			Tracks tracks;
			tracks.data.emplace_back();
			tracks.data[0].instrument = std::make_unique<SinInstrument>();
			tracks.data[0].sections = {{sections[0], 1.0}};

			Player p;

			bool spaceDown = false;
			while (w.exists())
			{
				// for (auto wke = w.keyboard.getEvent(); wke.type != WKET_INVALID; wke = w.keyboard.getEvent())
				// {
				// 	if (wke.type == WKET_KEYDOWN && wke.key == VK_SPACE)
				// 		p.start(tracks);
				// 	else if (wke.type == WKET_KEYUP && wke.key == VK_SPACE)
				// 		p.stop();
				// }
				if (w.keyboard.keyDown(VK_SPACE) && !spaceDown)
				{
					spaceDown = true;
					p.start(tracks);
				}
				else if (!w.keyboard.keyDown(VK_SPACE) && spaceDown)
				{
					spaceDown = false;
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