#include "window.h"

const WindowClass defWindowClass(L"defWindowClass", reinterpret_cast<HBRUSH>(COLOR_BACKGROUND), LoadCursorW(nullptr, IDC_ARROW));

void updateAllWindows()
{
	MSG msg;
	if (GetMessageW(&msg, nullptr, 0, 0) == -1) throw WinError(L"Failed to get message");
	TranslateMessage(&msg);
	DispatchMessageW(&msg);
}

WindowClass::WindowClass(std::wstring className, HBRUSH backgroundColour, HCURSOR cursor)
	: className(std::move(className)),
	  registered(false)
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = &Window::WindowProc;
	wc.hInstance = GetModuleHandleW(nullptr);
	wc.lpszClassName = this->className.c_str();
	wc.hbrBackground = backgroundColour + 1;
	wc.hCursor = cursor;

	registered = (RegisterClassExW(&wc) != 0U);
}

WindowClass::~WindowClass()
{
	if (registered) UnregisterClassW(className.c_str(), GetModuleHandleW(nullptr));
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		SetLastError(0);
		if ((SetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA,
			(long long)reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams
		) == 0) && (GetLastError() != 0U)) return -1;
		return 0;
	}

	auto* const window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if ((bool)window)
	{
		switch (msg)
		{
			case WM_CLOSE:
			{
				window->hWnd = nullptr;
			}
			return 0;
			case WM_DESTROY:
			{
				/* Om fönstret har förstörts av något annat än window->hWnd:s destruktor så
				   ser det här till att destruktorn inte också gör det. (Annars gör det här
				   ingen skillnad.) */
				(void)window->hWnd.release();
			}
			return 0;
		}

		try
		{
			return window->wndProc(msg, wParam, lParam);
		}
		catch (...)
		{
			lippincott();
			std::exit(0);
		}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

Window::Window(
	const WindowClass& wc,
	DWORD style,
	DWORD exStyle,
	const wchar_t* name,
	HWND parent,
	int width,
	int height
)
{
	if (!wc.registered) throw NoResWinError(L"Failed to register window class");
	hWnd = UWnd(CreateWindowExW(
		exStyle,
		wc.className.c_str(),
		name,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		parent,
		nullptr,
		GetModuleHandleW(nullptr),
		this
	));
	if (!hWnd) throw WinError(L"Failed to create window");
	ShowWindow(hWnd.get(), SW_SHOW);
}

Window::Window(
	const WindowClass& wc,
	DWORD style,
	DWORD exStyle,
	const wchar_t* name,
	Menu&& menu,
	HWND parent,
	int width,
	int height
)
{
	if (!wc.registered) throw NoResWinError(L"Failed to register window class");
	hWnd = UWnd(CreateWindowExW(
		exStyle,
		wc.className.c_str(),
		name,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		parent,
		menu.menu.get(),
		GetModuleHandleW(nullptr),
		this
	));
	if (!hWnd) throw WinError(L"Failed to create window");
	(void)menu.menu.release();
	ShowWindow(hWnd.get(), SW_SHOW);
}

void Window::update()
{
	MSG msg;
	if (GetMessageW(&msg, hWnd.get(), 0, 0) == -1) throw WinError(L"Failed to get message");
	TranslateMessage(&msg);
	DispatchMessageW(&msg);
}

LRESULT Control::subclassProc(HWND /*hWnd*/, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR /*idSubclass*/, DWORD_PTR refData)
{
	if (msg == WM_DESTROY)
	{
		(void)reinterpret_cast<Control*>(refData)->hWnd.release();
		return TRUE;
	}
	return reinterpret_cast<Control*>(refData)->proc(msg, wParam, lParam);
}

Control::Control(const wchar_t* wc, DWORD style, DWORD exStyle, const wchar_t* name, HWND parent)
{
	hWnd = UWnd(CreateWindowExW(
		exStyle,
		wc,
		name,
		style ^ WS_VISIBLE ^ WS_CHILD,
		0,
		0,
		0,
		0,
		parent,
		nullptr,
		GetModuleHandleW(nullptr),
		nullptr
	));
	if (!hWnd) throw WinError(L"Failed to create control");

	if (SetWindowSubclass(hWnd.get(), subclassProc, 1, (DWORD_PTR)this) == 0)
		throw WinError(L"Failed to change the control's window subclass");
}

std::wstring Control::getText()
{
	std::wstring s;
	const size_t size = SendMessageW(*this, WM_GETTEXTLENGTH, 0, 0) + 1;
	if (size != 1)
	{
		s.resize(size);
		s.resize(SendMessageW(*this, WM_GETTEXT, s.size(), reinterpret_cast<LPARAM>(&s[0])));
	}
	return s;
}

uint32_t IpAddress::getAddress()
{
	uint32_t addr = 0;
	SendMessageW(*this, IPM_GETADDRESS, 0, reinterpret_cast<LPARAM>(&addr));
	return addr;
}

void UpDown::setRange(int min, int max)
{
	SendMessageW(*this, UDM_SETRANGE32, min, max);
}

UpDown::UpDown(DWORD style, DWORD exStyle, HWND buddy, HWND parent)
	: UpDown(style, exStyle, parent)
{
	SendMessageW(*this, UDM_SETBUDDY, reinterpret_cast<WPARAM>(buddy), 0);
}

int UpDown::getValue()
{
	BOOL success = FALSE;
	int ret = SendMessageW(*this, UDM_GETPOS32, 0, reinterpret_cast<LPARAM>(&success));
	if (success != FALSE) throw WinError(L"Failed to get value from number entry box");
	return ret;
}

Menu::Menu(std::initializer_list<MenuItemVariant> elements, HMENU* copy)
	: menu(CreateMenu())
{
	if (!menu) throw WinError(L"Failed to create menu");

	for (const auto& e : elements)
	{
		std::visit([&](const auto& e){
			using T = std::decay_t<decltype(e)>;
			if constexpr (std::is_same_v<T, MenuItem::String>)
			{
				if (AppendMenuW(
					menu.get(),
					MF_STRING,
					e.id,
					e.text.c_str()
				) == 0) throw WinError(L"Failed to create menu item");
			}
			else if constexpr (std::is_same_v<T, MenuItem::SubMenu>)
			{
				if (AppendMenuW(
					menu.get(),
					MF_POPUP,
					(UINT_PTR)(HMENU)e.menu.menu.get(),
					e.text.c_str()
				) == 0) throw WinError(L"Failed to create menu item");
				(void)e.menu.menu.release();
			}
			else if constexpr (std::is_same_v<T, MenuItem::Separator>)
			{
				if (AppendMenuW(
					menu.get(),
					MF_SEPARATOR,
					0,
					nullptr
				) == 0) throw WinError(L"Failed to create menu item");
			}
			else if constexpr (std::is_same_v<T, MenuItem::CheckButton>)
			{
				if (AppendMenuW(
					menu.get(),
					MF_STRING,
					e.id,
					e.text.c_str()
				) == 0) throw WinError(L"Failed to create menu item");

				CheckMenuItem(
					menu.get(),
					e.id,
					e.checked ? MF_CHECKED : MF_UNCHECKED
				);
			}
			else if constexpr (std::is_same_v<T, MenuItem::RadioButton>)
			{
				if (AppendMenuW(
					menu.get(),
					MF_STRING,
					e.id,
					e.text.c_str()
				) == 0) throw WinError(L"Failed to create menu item");

				MENUITEMINFOW mii{};
				mii.cbSize = sizeof(mii);
				mii.fMask = MIIM_FTYPE | MIIM_STATE;

				if (GetMenuItemInfoW(
					menu.get(),
					e.id,
					MF_BYCOMMAND,
					&mii
				) == 0) throw WinError(L"Failed to get menu item info");

				if (e.checked) mii.fType |= MFT_RADIOCHECK;
				mii.fState = e.checked ? MFS_CHECKED : MFS_UNCHECKED;

				if (SetMenuItemInfoW(
					menu.get(),
					e.id,
					MF_BYCOMMAND,
					&mii
				) == 0) throw WinError(L"Failed to get menu item info");
			}
		}, e);
	}
	
	if (copy != nullptr) *copy = menu.get();
}