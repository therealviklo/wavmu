#include "gui.h"

LRESULT TrackView::wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			SolidBrush grey(D2D1::ColorF(0.1f, 0.1f, 0.1f), mw.rt);
			SolidBrush lightgrey(D2D1::ColorF(0.2f, 0.2f, 0.2f), mw.rt);

			const auto size = mw.getSize();

			mw.rt.beginDraw();
			mw.rt.clear(0.05f, 0.05f, 0.05f);

			{
				const std::lock_guard lg(mw.tracks.mtx);
				size_t i = 0;
				for (; i < mw.tracks.data.size(); i++)
				{
					mw.rt.drawRectangle(
						D2D1::RectF(0.0f, 100.0f * i, 200.0f, 100.0f * i + 100.0f),
						grey
					);
					mw.rt.drawLine(
						D2D1::Point2F(0.0f, 100.0f * (i + 1)),
						D2D1::Point2F(size.right, 100.0f * (i + 1)),
						grey,
						2.0f
					);
				}
				mw.rt.drawRectangle(
					D2D1::RectF(0.0f, 100.0f * i, 200.0f, 100.0f * i + 100.0f),
					lightgrey
				);
			}

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
	vpush(std::make_unique<TrackView>());
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