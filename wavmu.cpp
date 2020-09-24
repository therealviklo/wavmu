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
				{0.0, 1/8., 60},
				{1/8., 1/8., 60},
				{2/8., 2/8., 72},
				{4/8., 3/8., 67},
				{7/8., 2/8., 66},
				
				{9/8., 2/8., 65},
				{11/8., 2/8., 63},
				{13/8., 1/8., 60},
				{14/8., 1/8., 63},
				{15/8., 1/8., 65},
				

				{0.0 + 1 * 2.0, 1/8., 58},
				{1/8. + 1 * 2.0, 1/8., 58},
				{2/8. + 1 * 2.0, 2/8., 72},
				{4/8. + 1 * 2.0, 3/8., 67},
				{7/8. + 1 * 2.0, 2/8., 66},
				
				{9/8. + 1 * 2.0, 2/8., 65},
				{11/8. + 1 * 2.0, 2/8., 63},
				{13/8. + 1 * 2.0, 1/8., 60},
				{14/8. + 1 * 2.0, 1/8., 63},
				{15/8. + 1 * 2.0, 1/8., 65},
				

				{0.0 + 2 * 2.0, 1/8., 57},
				{1/8. + 2 * 2.0, 1/8., 57},
				{2/8. + 2 * 2.0, 2/8., 72},
				{4/8. + 2 * 2.0, 3/8., 67},
				{7/8. + 2 * 2.0, 2/8., 66},
				
				{9/8. + 2 * 2.0, 2/8., 65},
				{11/8. + 2 * 2.0, 2/8., 63},
				{13/8. + 2 * 2.0, 1/8., 60},
				{14/8. + 2 * 2.0, 1/8., 63},
				{15/8. + 2 * 2.0, 1/8., 65},
				

				{0.0 + 3 * 2.0, 1/8., 56},
				{1/8. + 3 * 2.0, 1/8., 56},
				{2/8. + 3 * 2.0, 2/8., 72},
				{4/8. + 3 * 2.0, 3/8., 67},
				{7/8. + 3 * 2.0, 2/8., 66},
				
				{9/8. + 3 * 2.0, 2/8., 65},
				{11/8. + 3 * 2.0, 2/8., 63},
				{13/8. + 3 * 2.0, 1/8., 60},
				{14/8. + 3 * 2.0, 1/8., 63},
				{15/8. + 3 * 2.0, 1/8., 65},
			}})};

			Tracks tracks;
			tracks.data.emplace_back();
			tracks.data[0].instrument = std::make_unique<SinInstrument>();
			tracks.data[0].sections = {{sections[0], 0.0}};

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