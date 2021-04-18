#pragma once
#include <concepts>
#include <memory>
#include <stack>
#include "window.h"
#include "player.h"
#include "d2d.h"

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