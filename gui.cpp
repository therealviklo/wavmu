#include "gui.h"

TrackWindow::TrackWindow(MainWindow& mw) :
	Window(
		defWindowClass,
		WS_CHILD,
		0,
		L"",
		mw,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		false
	),
	tracks(mw.tracks),
	player(mw.player)
{
	const auto size = mw.getSize();
	SetWindowPos(
		*this,
		nullptr,
		0,
		0,
		size.right,
		size.bottom,
		SWP_NOOWNERZORDER
		| SWP_NOZORDER
	);
}

LRESULT TrackWindow::wndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(*this, &ps);
			UHandle<HBRUSH, DeleteObject> brush(CreateSolidBrush(RGB(255, 0, 0)));

			FillRect(hdc, &ps.rcPaint, brush.get());

			EndPaint(*this, &ps);
		}
		return 0;
	}
	return DefWindowProcW(*this, msg, wParam, lParam);
}

MainWindow::MainWindow() :
	Window(
		defWindowClass,
		WS_OVERLAPPEDWINDOW,
		0,
		L"Wavmu"
	)
{
	wstack.push(std::make_unique<TrackWindow>(*this));
}
	
void MainWindow::onResize(WORD w, WORD h)
{
	if (!wstack.empty())
	{
		SetWindowPos(
			wstack.top(),
			nullptr,
			0,
			0,
			w,
			h,
			SWP_NOOWNERZORDER
			| SWP_NOZORDER
		);
	}
}

LRESULT MainWindow::wndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
		{
			onResize(LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	}
	return DefWindowProcW(*this, msg, wParam, lParam);
}