#include "gui.h"

LRESULT TrackWindow::wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			mw.rt.beginDraw();
			mw.rt.clear(0.1f, 0.1f, 0.1f);

			if (mw.rt.endDraw(mw.d2dfac))
				ValidateRect(mw, nullptr);
		}
		return 0;
	}
	return DefWindowProcW(mw, msg, wParam, lParam);
}

MainWindow::MainWindow() :
	Window(
		defWindowClass,
		WS_OVERLAPPEDWINDOW,
		0,
		L"Wavmu",
		nullptr,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		false
	),
	rt(*this, d2dfac)
{
	vpush(std::make_unique<TrackWindow>());
	ShowWindow(*this, SW_SHOW);
}

void MainWindow::vpush(std::unique_ptr<View> view)
{
	views.emplace(std::move(view));
}

void MainWindow::vpop() noexcept
{
	views.pop();
}

LRESULT MainWindow::wndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	}
	if (!views.empty())
		return views.top()->wndProc(*this, msg, wParam, lParam);
	return DefWindowProcW(*this, msg, wParam, lParam);
}