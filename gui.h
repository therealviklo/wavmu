#pragma once
#include "window.h"
#include "wm.h"
#include "player.h"

class MainWindow;

class TrackWindow : public Window
{
private:
	Tracks& tracks;
	Player& player;
public:
	TrackWindow(MainWindow& mw);

	LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam);
};

class MainWindow : public Window
{
	friend TrackWindow::TrackWindow(MainWindow&);
private:
	Tracks tracks;
	Player player;
	WindowStack wstack;
public:
	MainWindow();
	
	void onResize(WORD w, WORD h);

	LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam);
};