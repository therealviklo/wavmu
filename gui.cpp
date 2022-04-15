#include "gui.h"

const WindowClass specWindowClass(
	L"specWindowClass",
	reinterpret_cast<HBRUSH>(COLOR_BACKGROUND),
	LoadCursorW(nullptr, IDC_ARROW),
	CS_DBLCLKS
);

MainWindow::RSC::RSC(WICFactory& wicfac, RenderTarget& rt)
{
	from = RSC_TONER;
	constexpr size_t to = RSC_TONER;

	const HMODULE mod = GetModuleHandleW(nullptr);
	if (!mod) throw WinError(L"Failed to get module handle");

	bmps.reserve(to - from + 1);
	for (size_t i = from; i <= to; ++i)
	{
		const HRSRC rsrc = FindResourceW(mod, MAKEINTRESOURCEW(i), MAKEINTRESOURCEW(RSCT_DATA));
		if (!rsrc) throw WinError(L"Failed to find resource");
		const HGLOBAL glob = LoadResource(mod, rsrc);
		if (!glob) throw WinError(L"Failed to load resource");
		const void* const data = LockResource(glob);
		if (!data) throw NoResWinError(L"Failed to get resource data pointer");
		const UINT size = SizeofResource(mod, rsrc);
		if (!size) throw WinError(L"Failed to get resource size");
		bmps.emplace_back((const char*)data, size, wicfac, rt);
	}
}

MainWindow::MainWindow() :
	Window(
		specWindowClass,
		WS_OVERLAPPEDWINDOW,
		0,
		L"Wavmu",
		Menu({
			MenuItem::SubMenu{
				L"Arkiv",
				Menu({
					MenuItem::String{L"Spara projekt\tCtrl+S", ID::saveProject},
					MenuItem::String{L"Spara projekt som...", ID::saveProjectAs},
					MenuItem::String{L"Öppna projekt...", ID::openProject},
					MenuItem::String{L"Exportera som...", ID::exportSong}
				})
			}}),
		nullptr,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		false
	),
	rt(*this, d2dfac),
	rsc(wicfac, rt) {}

void MainWindow::vpush(std::unique_ptr<View> view)
{
	views.emplace(std::move(view));
	InvalidateRect(*this, nullptr, FALSE);
}

void MainWindow::vpop() noexcept
{
	views.pop();
	InvalidateRect(*this, nullptr, FALSE);
}

LRESULT MainWindow::wndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
		{
			if (!views.empty())
				views.top()->onResize(LOWORD(lParam), HIWORD(lParam));
			rt.resize(LOWORD(lParam), HIWORD(lParam));
			InvalidateRect(*this, nullptr, FALSE);
		}
		return 0;
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == 0)
			{
				switch (LOWORD(wParam))
				{
					case ID::saveProject:
					{
						const std::optional<std::string> file =
							prevSaveName.empty() ? fileSaveDialogue(L"wmu") : prevSaveName;
						if (file)
						{
							try
							{
								prevSaveName = *file;
								saveSong(tracks, file->c_str());
							}
							catch (...)
							{
								lippincottNonFatal(L"Kunde inte spara projekt");
							}
						}
					}
					return 0;
					case ID::saveProjectAs:
					{
						const std::optional<std::string> file = fileSaveDialogue(L"wmu");
						if (file)
						{
							try
							{
								prevSaveName = *file;
								saveSong(tracks, file->c_str());
							}
							catch (...)
							{
								lippincottNonFatal(L"Kunde inte spara projekt");
							}
						}
					}
					return 0;
					case ID::openProject:
					{
						const std::optional<std::string> file = fileOpenDialogue(L"wmu");
						if (file)
						{
							try
							{
								loadSong(tracks, file->c_str());
								InvalidateRect(*this, nullptr, FALSE);
							}
							catch (...)
							{
								lippincottNonFatal(L"Kunde inte öppna projekt");
							}
						}
					}
					return 0;
					case ID::exportSong:
					{
						const std::optional<std::string> file = fileSaveDialogue(L"wav");
						if (file)
						{
							try
							{
								exportSong(tracks, 240, file->c_str());
							}
							catch (...)
							{
								lippincottNonFatal(L"Kunde inte exportera låt");
							}
						}
					}
					return 0;
				}
			}
		}
		break;
	}
	if (!views.empty())
		return views.top()->wndProc(*this, msg, wParam, lParam);
	return DefWindowProcW(*this, msg, wParam, lParam);
}