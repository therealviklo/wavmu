#include "gui.h"

const WindowClass specWindowClass(
	L"specWindowClass",
	reinterpret_cast<HBRUSH>(COLOR_BACKGROUND),
	LoadCursorW(nullptr, IDC_ARROW),
	CS_DBLCLKS
);

MainWindow::MainWindow() :
	Window(
		specWindowClass,
		WS_OVERLAPPEDWINDOW,
		0,
		L"Wavmu",
		nullptr,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		false
	),
	rt(*this, d2dfac) {}

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
		case WM_SIZE:
		{
			rt.resize(LOWORD(lParam), HIWORD(lParam));
			InvalidateRect(*this, nullptr, FALSE);
		}
		return 0;
	}
	if (!views.empty())
		return views.top()->wndProc(*this, msg, wParam, lParam);
	return DefWindowProcW(*this, msg, wParam, lParam);
}