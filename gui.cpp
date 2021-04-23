#include "gui.h"

namespace
{
	inline Placement channelHeadPlace(size_t n)
	{
		return {0.0f, 100.0f * n, 200.0f, 100.0f};
	}
}

bool pressInPlace(float x, float y, Placement place)
{
	return x > place.x
		&& x < place.x + place.w
		&& y > place.y
		&& y < place.y + place.h;
}

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
					const auto place = channelHeadPlace(i);
					mw.rt.drawRectangle(
						place,
						grey
					);
					mw.rt.drawLine(
						D2D1::Point2F(0.0f, place.y + place.h),
						D2D1::Point2F(size.right, place.y + place.h),
						grey,
						2.0f
					);
				}
				const auto place = channelHeadPlace(i);
				mw.rt.drawRectangle(
					place,
					lightgrey
				);
				mw.rt.drawRectangle(
					Placement{
						place.x + place.w / 2.0f - 15.0f / 2.0f,
						place.y + place.h / 4.0f,
						15.0f,
						place.h / 2.0f
					},
					grey
				);
				mw.rt.drawRectangle(
					Placement{
						place.x + place.w / 2.0f - place.h / 4.0f,
						place.y + place.h / 2.0f - 15.0f / 2.0f,
						place.h / 2.0f,
						15.0f
					},
					grey
				);
			}

			if (mw.rt.endDraw(mw.d2dfac))
				ValidateRect(mw, nullptr);
		}
		return 0;
		case WM_LBUTTONDOWN:
		{
			const auto mx = GET_X_LPARAM(lParam);
			const auto my = GET_Y_LPARAM(lParam);
			if (pressInPlace(mx, my, channelHeadPlace(mw.tracks.data.size())))
			{
				const std::lock_guard lg(mw.tracks.mtx);
				if (!mw.tracks.playing.test(std::memory_order::relaxed))
				{
					mw.tracks.data.push_back(Track{std::make_unique<SinInstrument>(), std::vector<Track::SectionRef>{}});
					InvalidateRect(mw, nullptr, FALSE);
				}
			}
			else break;
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