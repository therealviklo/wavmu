#pragma once
#include <concepts>
#include <memory>
#include <stack>
#include "window.h"
#include "player.h"
#include "d2d.h"
#include <windowsx.h>
#include "instrument.h"

class MainWindow;

class View
{
public:
	View() = default;
	virtual ~View() = default;

	View(const View&) = delete;
	View& operator=(const View&) = delete;

	virtual LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};

struct Placement
{
	float x;
	float y;
	float w;
	float h;

	operator D2D1_RECT_F() const noexcept
	{
		return D2D1::RectF(x, y, x + w, y + h);
	}
};

bool pressInPlace(float x, float y, Placement place);

class TrackView : public View
{
public:
	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};

class MainWindow : public Window
{
private:
	std::stack<std::unique_ptr<View>> views;
public:
	Tracks tracks;
	Player player;
	D2DFactory d2dfac;
	RenderTarget rt;

	MainWindow();
	
	void vpush(std::unique_ptr<View> view);
	void vpop() noexcept;

	LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam);
};