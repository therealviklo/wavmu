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
				L"&Arkiv",
				Menu({
					MenuItem::String{L"&Spara projekt\tCtrl+S", ID::saveProject},
					MenuItem::String{L"Spara &projekt som...\tCtrl+Shift+S", ID::saveProjectAs},
					MenuItem::String{L"Öppna pr&ojekt...\tCtrl+O", ID::openProject},
					MenuItem::String{L"&Exportera som...\tCtrl+E", ID::exportSong}
				})
			},
			MenuItem::SubMenu{
				L"&Tonart",
				Menu({
					MenuItem::SubMenu{
						L"&Dur",
						Menu({
							MenuItem::RadioButton{L"B (H)", false, ID::Bmajor},
							MenuItem::RadioButton{L"A#", false, ID::ASmajor},
							MenuItem::RadioButton{L"A", false, ID::Amajor},
							MenuItem::RadioButton{L"G#", false, ID::GSmajor},
							MenuItem::RadioButton{L"G", false, ID::Gmajor},
							MenuItem::RadioButton{L"F#", false, ID::FSmajor},
							MenuItem::RadioButton{L"F", false, ID::Fmajor},
							MenuItem::RadioButton{L"E", false, ID::Emajor},
							MenuItem::RadioButton{L"D#", false, ID::DSmajor},
							MenuItem::RadioButton{L"D", false, ID::Dmajor},
							MenuItem::RadioButton{L"C#", false, ID::CSmajor},
							MenuItem::RadioButton{L"C", false, ID::Cmajor}
						},
						majorKeyMenu)
					},
					MenuItem::SubMenu{
						L"&Moll",
						Menu({
							MenuItem::RadioButton{L"B (H)", false, ID::Bminor},
							MenuItem::RadioButton{L"A#", false, ID::ASminor},
							MenuItem::RadioButton{L"A", false, ID::Aminor},
							MenuItem::RadioButton{L"G#", false, ID::GSminor},
							MenuItem::RadioButton{L"G", false, ID::Gminor},
							MenuItem::RadioButton{L"F#", false, ID::FSminor},
							MenuItem::RadioButton{L"F", false, ID::Fminor},
							MenuItem::RadioButton{L"E", false, ID::Eminor},
							MenuItem::RadioButton{L"D#", false, ID::DSminor},
							MenuItem::RadioButton{L"D", false, ID::Dminor},
							MenuItem::RadioButton{L"C#", false, ID::CSminor},
							MenuItem::RadioButton{L"C", false, ID::Cminor}
						},
						minorKeyMenu)
					}
				})
			}}),
		nullptr,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		false
	),
	rt(*this, d2dfac),
	rsc(wicfac, rt),
	font1(
		dwfac,
		L"Arial",
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18.0f
	) {}

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
				auto uncheck = [&](HMENU menu, UINT item) -> void {
					MENUITEMINFOW mii{};
					mii.cbSize = sizeof(mii);
					mii.fMask = MIIM_STATE;
					mii.fState = MFS_UNCHECKED;
					if (!SetMenuItemInfoW(
							menu,
							item,
							FALSE,
							&mii
						))
						throw WinError(L"Unable to uncheck menu radio item");
				};
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
								prevSaveName = *file;
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
					case ID::Bmajor:
					case ID::ASmajor:
					case ID::Amajor:
					case ID::GSmajor:
					case ID::Gmajor:
					case ID::FSmajor:
					case ID::Fmajor:
					case ID::Emajor:
					case ID::DSmajor:
					case ID::Dmajor:
					case ID::CSmajor:
					case ID::Cmajor:
					{
						const UINT state =
							GetMenuState(
								majorKeyMenu,
								LOWORD(wParam),
								MF_BYCOMMAND
							);
						if (state == static_cast<decltype(state)>(-1))
							throw NoResWinError(L"Unable to get menu state");
						if (state & MF_CHECKED)
						{
							uncheck(majorKeyMenu, LOWORD(wParam));
							keyGuide.reset();
						}
						else
						{
							if (keyGuide && !keyGuide->major)
							{
								uncheck(
									minorKeyMenu,
									static_cast<unsigned>(ID::Cminor) - keyGuide->note
								);
							}
							if (!CheckMenuRadioItem(
									majorKeyMenu,
									ID::Bmajor,
									ID::Cmajor,
									LOWORD(wParam),
									MF_BYCOMMAND
								))
								throw WinError(L"Unable to check menu radio item");
							keyGuide.emplace(
								Key{
									true,
									static_cast<unsigned>(ID::Cmajor) - static_cast<unsigned>(LOWORD(wParam))
								}
							);
						}
						InvalidateRect(*this, nullptr, FALSE);
					}
					return 0;
					case ID::Bminor:
					case ID::ASminor:
					case ID::Aminor:
					case ID::GSminor:
					case ID::Gminor:
					case ID::FSminor:
					case ID::Fminor:
					case ID::Eminor:
					case ID::DSminor:
					case ID::Dminor:
					case ID::CSminor:
					case ID::Cminor:
					{
						const UINT state =
							GetMenuState(
								minorKeyMenu,
								LOWORD(wParam),
								MF_BYCOMMAND
							);
						if (state == static_cast<decltype(state)>(-1))
							throw NoResWinError(L"Unable to get menu state");
						if (state & MF_CHECKED)
						{
							uncheck(minorKeyMenu, LOWORD(wParam));
							keyGuide.reset();
						}
						else
						{
							if (keyGuide && keyGuide->major)
							{
								uncheck(
									majorKeyMenu,
									static_cast<unsigned>(ID::Cmajor) - keyGuide->note
								);
							}
							if (!CheckMenuRadioItem(
									minorKeyMenu,
									ID::Bminor,
									ID::Cminor,
									LOWORD(wParam),
									MF_BYCOMMAND
								))
								throw WinError(L"Unable to check menu radio item");
							keyGuide.emplace(
								Key{
									false,
									static_cast<unsigned>(ID::Cminor) - static_cast<unsigned>(LOWORD(wParam))
								}
							);
						}
						InvalidateRect(*this, nullptr, FALSE);
					}
					return 0;
				}
			}
			else if (HIWORD(wParam) == 1)
			{
				switch (LOWORD(wParam))
				{
					case ACC_SAVE:
					{
						SendMessageW(*this, WM_COMMAND, ID::saveProject, 0);
					}
					return 0;
					case ACC_SAVEAS:
					{
						SendMessageW(*this, WM_COMMAND, ID::saveProjectAs, 0);
					}
					return 0;
					case ACC_OPEN:
					{
						SendMessageW(*this, WM_COMMAND, ID::openProject, 0);
					}
					return 0;
					case ACC_EXPORT:
					{
						SendMessageW(*this, WM_COMMAND, ID::exportSong, 0);
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