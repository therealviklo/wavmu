#pragma once
#include <concepts>
#include <memory>
#include <stack>
#include "window.h"
#include "player.h"
#include "d2d.h"
#include <windowsx.h>
#include "instrument.h"
#include "guiutils.h"
#include "rsc.h"
#include "save.h"
#include "filedialogue.h"
#include "export.h"
#include "id.h"

class MainWindow;

class View
{
public:
	View() = default;
	virtual ~View() = default;

	View(const View&) = delete;
	View& operator=(const View&) = delete;

	virtual LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void onResize(WORD /*width*/, WORD /*height*/) {}
};

class MainWindow : public Window
{
private:
	std::stack<std::unique_ptr<View>> views;

	std::string prevSaveName;
public:
	Tracks tracks;
	Player player;
	D2DFactory d2dfac;
	WICFactory wicfac;
	RenderTarget rt;
	struct RSC
	{
	private:
		std::vector<Bitmap> bmps;
		size_t from;
	public:
		RSC(WICFactory& wicfac, RenderTarget& rt);

		constexpr Bitmap& operator[](size_t i) noexcept
		{
			return bmps[i - from];
		}
	} rsc;

	MainWindow();
	
	void vpush(std::unique_ptr<View> view);
	void vpop() noexcept;

	LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam);
};