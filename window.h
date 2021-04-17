#pragma once
#include <variant>
#include "win.h"
#include <commctrl.h>
#include "utils.h"
#include "winerror.h"
#include "lippincott.h"

// Klasser för att göra fönster.

void updateAllWindows();

using UWnd = UHandle<HWND, DestroyWindow>;

// Forwarddeklaration
class Window;

/* RAII-wrapper för en window class (som inte är en C++-klass utan något
   som Windows använder). En global default-window-class deklareras senare
   i filen. */
class WindowClass
{
	friend Window;
private:
	std::wstring className;
	bool registered;
public:
	WindowClass(std::wstring className, HBRUSH backgroundColour, HCURSOR cursor);
	~WindowClass();

	WindowClass(const WindowClass&) = delete;
	WindowClass& operator=(const WindowClass&) = delete;
};

class Menu;

/* En klass som representerar ett generiskt fönster. Det är inte meningen att man direkt
   ska skapa instanser av klassen Window utan att man ska göra en underklass till Window
   och instantiera den (se gui.h). */
class Window
{
	friend WindowClass;
private:
	UWnd hWnd;
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Window() noexcept = default;
	Window(
		const WindowClass& wc,
		DWORD style,
		DWORD exStyle,
		const wchar_t* name,
		HWND parent = nullptr,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT
	);
	Window(
		const WindowClass& wc,
		DWORD style,
		DWORD exStyle,
		const wchar_t* name,
		Menu&& menu,
		HWND parent = nullptr,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT
	);
	virtual ~Window() = default;

	/* En virtuell funktion som får parametrarna från den "riktiga" WndProc:en. Det är meningen att underklasserna
	   ska överskrida den här funktionen. */
	virtual LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam) { return DefWindowProcW(hWnd.get(), msg, wParam, lParam); }

	// Kör den här för att få fönstret att uppdatera. (Annars fryser det.)
	void update();

	template <class ParentWindowClass>
	ParentWindowClass& getParent() const
	{
		return dynamic_cast<MainWindow&>(*reinterpret_cast<Window*>(GetWindowLongPtrW(GetParent(hWnd.get()), GWLP_USERDATA)));
	}

	RECT getSize() const
	{
		RECT rc{};
		if (!GetClientRect(hWnd.get(), &rc))
			throw WinError(L"Failed to get client area");
		return rc;
	}

	constexpr operator HWND() noexcept { return hWnd.get(); }
	constexpr operator bool() const noexcept { return hWnd.get(); }
};

class Control
{
private:
	UHandle<HWND, DestroyWindow> hWnd;

	static LRESULT subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass, DWORD_PTR refData);
protected:
	virtual LRESULT proc(UINT msg, WPARAM wParam, LPARAM lParam) { return DefSubclassProc(*this, msg, wParam, lParam); }
public:
	Control(const wchar_t* wc, DWORD style, DWORD exStyle, const wchar_t* name, HWND parent);
	virtual ~Control() = default;

	std::wstring getText();

	constexpr operator HWND() noexcept { return hWnd.get(); }
	constexpr operator bool() const noexcept { return hWnd.get(); }
};

class EditControl : public Control
{
public:
	EditControl(DWORD style, DWORD exStyle, HWND parent)
		: Control(L"Edit", style, exStyle, nullptr, parent) {}
};

class Button : public Control
{
public:
	Button(const wchar_t* text, DWORD style, DWORD exStyle, HWND parent)
		: Control(L"Button", style, exStyle, text, parent) {}
};

class IpAddress : public Control
{
public:
	IpAddress(DWORD style, DWORD exStyle, HWND parent)
		: Control(L"SysIPAddress32", style, exStyle, nullptr, parent) {}

	uint32_t getAddress();
};

class UpDown : public Control
{
public:
	UpDown(DWORD style, DWORD exStyle, HWND parent)
		: Control(L"msctls_updown32", style, exStyle, nullptr, parent) {}
	UpDown(DWORD style, DWORD exStyle, HWND buddy, HWND parent);

	void setRange(int min, int max);
	int getValue();
};

class Label : public Control
{
public:
	Label(const wchar_t* text, DWORD style, DWORD exStyle, HWND parent)
		: Control(L"Static", style, exStyle, text, parent) {}
};

namespace MenuItem
{
	struct SubMenu;
	struct String
	{
		std::wstring text;
		UINT_PTR id;
	};
	struct Separator {};
	struct CheckButton
	{
		std::wstring text;
		bool checked;
		UINT_PTR id;
	};
	struct RadioButton
	{
		std::wstring text;
		bool checked;
		UINT_PTR id;
	};
}
using MenuItemVariant = std::variant<
	MenuItem::String,
	MenuItem::SubMenu,
	MenuItem::Separator,
	MenuItem::CheckButton,
	MenuItem::RadioButton
>;
class Menu
{
	friend Window;
private:
	mutable UHandle<HMENU, DestroyMenu> menu;
	Menu(std::initializer_list<MenuItemVariant> elements, HMENU* copy);
public:
	Menu(std::initializer_list<MenuItemVariant> elements)
		: Menu(std::move(elements), nullptr) {}
	Menu(std::initializer_list<MenuItemVariant> elements, HMENU& copy)
		: Menu(std::move(elements), &copy) {}
};
struct MenuItem::SubMenu
{
	std::wstring text;
	Menu menu;
};

template <class W, void (*callback)(W&) = nullptr>
class Timer
{
private:
	bool started;
	UINT_PTR id;
	HWND hWnd;
	UINT msTimeout;

	static void timerproc(HWND hWnd, UINT msg, UINT_PTR id, DWORD ms) noexcept
	{
		try
		{
			callback(dynamic_cast<W&>(*reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA))));
		}
		catch (...)
		{
			lippincott();
			std::terminate();
		}
	}
public:
	Timer(UINT_PTR id, HWND hWnd, UINT msTimeout) noexcept :
		started(false),
		id(id),
		hWnd(hWnd),
		msTimeout(msTimeout) {}
	~Timer()
	{
		if (started) KillTimer(hWnd, id);
	}

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	void start()
	{
		if (!started)
		{
			if (SetTimer(
				hWnd,
				id,
				msTimeout,
				callback != nullptr ? &timerproc : nullptr
			) == 0) throw WinError(L"Failed to start timer");
			started = true;
		}
	}

	void stop()
	{
		if (started)
		{
			if (KillTimer(
				hWnd,
				id
			) == FALSE) throw WinError(L"Failed to stop timer");
			started = false;
		}
	}

	constexpr bool isStarted() const noexcept { return started; }

	constexpr HWND getHwnd() const noexcept { return hWnd; }
};

extern const WindowClass defWindowClass;