#include "gui.h"
#include "trackview.h"

// class TestWindow : public Window
// {
// private:
//	Sections sections;
//	Tracks tracks;
//	Player player;
// public:
// 	TestWindow()
// 		: Window(
// 			defWindowClass,
// 			WS_OVERLAPPEDWINDOW,
// 			WS_EX_OVERLAPPEDWINDOW,
// 			L"Wavmu"
// 		  ),
// 		  sections{std::make_shared<Section>(Section{{
// 			{0.0, 1/8., 60},
// 			{1/8., 1/8., 60},
// 			{2/8., 2/8., 72},
// 			{4/8., 3/8., 67},
// 			{7/8., 2/8., 66},
			
// 			{9/8., 2/8., 65},
// 			{11/8., 2/8., 63},
// 			{13/8., 1/8., 60},
// 			{14/8., 1/8., 63},
// 			{15/8., 1/8., 65},
			

// 			{0.0 + 1 * 2.0, 1/8., 58},
// 			{1/8. + 1 * 2.0, 1/8., 58},
// 			{2/8. + 1 * 2.0, 2/8., 72},
// 			{4/8. + 1 * 2.0, 3/8., 67},
// 			{7/8. + 1 * 2.0, 2/8., 66},
			
// 			{9/8. + 1 * 2.0, 2/8., 65},
// 			{11/8. + 1 * 2.0, 2/8., 63},
// 			{13/8. + 1 * 2.0, 1/8., 60},
// 			{14/8. + 1 * 2.0, 1/8., 63},
// 			{15/8. + 1 * 2.0, 1/8., 65},
			

// 			{0.0 + 2 * 2.0, 1/8., 57},
// 			{1/8. + 2 * 2.0, 1/8., 57},
// 			{2/8. + 2 * 2.0, 2/8., 72},
// 			{4/8. + 2 * 2.0, 3/8., 67},
// 			{7/8. + 2 * 2.0, 2/8., 66},
			
// 			{9/8. + 2 * 2.0, 2/8., 65},
// 			{11/8. + 2 * 2.0, 2/8., 63},
// 			{13/8. + 2 * 2.0, 1/8., 60},
// 			{14/8. + 2 * 2.0, 1/8., 63},
// 			{15/8. + 2 * 2.0, 1/8., 65},
			

// 			{0.0 + 3 * 2.0, 1/8., 56},
// 			{1/8. + 3 * 2.0, 1/8., 56},
// 			{2/8. + 3 * 2.0, 2/8., 72},
// 			{4/8. + 3 * 2.0, 3/8., 67},
// 			{7/8. + 3 * 2.0, 2/8., 66},
			
// 			{9/8. + 3 * 2.0, 2/8., 65},
// 			{11/8. + 3 * 2.0, 2/8., 63},
// 			{13/8. + 3 * 2.0, 1/8., 60},
// 			{14/8. + 3 * 2.0, 1/8., 63},
// 			{15/8. + 3 * 2.0, 1/8., 65},
// 		  }})}
// 	{
// 		tracks.data.emplace_back();
// 		tracks.data[0].instrument = nullptr;
// 		tracks.data[0].sections = {{sections[0], 0.0}};
// 	}

// 	LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam) override
// 	{
// 		switch (msg)
// 		{
// 			static bool spaceDown = false;
// 			case WM_KEYDOWN:
// 			{
// 				switch (wParam)
// 				{
// 					case VK_SPACE:
// 					{
// 						if (!spaceDown)
// 						{
// 							spaceDown = true;
// 							player.start(tracks, 240.0);
// 						}
// 					}
// 					break;
// 				}
// 			}
// 			return 0;
// 			case WM_KEYUP:
// 			{
// 				switch (wParam)
// 				{
// 					case VK_SPACE:
// 					{
// 						if (spaceDown)
// 						{
// 							spaceDown = false;
// 							player.stop();
// 						}
// 					}
// 					break;
// 				}
// 			}
// 			return 0;
// 		}
// 		return DefWindowProcW(*this, msg, wParam, lParam);
// 	}
// };

class ComHandler
{
public:
	ComHandler()
	{
		hrthrow(CoInitializeEx(nullptr, COINIT_MULTITHREADED),
				L"Failed to initialise COM");
	}

	~ComHandler()
	{
		CoUninitialize();
	}

	ComHandler(const ComHandler&) = delete;
	ComHandler& operator=(const ComHandler&) = delete;
};

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PWSTR /*pCmdLine*/, int /*nCmdShow*/)
{
	try
	{
		ComHandler ch;
		
		MainWindow mw;
		mw.vpush(std::make_unique<TrackView>());
		ShowWindow(mw, SW_SHOWDEFAULT);
		while (mw)
		{
			updateAllWindows();
		}
	}
	catch (...)
	{
		lippincott();
	}

	return 0;
}