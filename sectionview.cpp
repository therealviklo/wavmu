#include "sectionview.h"

constexpr float noteH = 50.0f;

void SectionView::captureScroll(RECT size)
{
	constexpr float pianoRollH = noteH * 256.0f;
	if (size.bottom >= pianoRollH)
	{
		scroll.y = 0.0f;
	}
	else
	{
		scroll.y = std::clamp<float>(scroll.y, size.bottom - pianoRollH, 0.0f);
	}
}

SectionView::SectionView(Section& sect) noexcept :
	sect(sect),
	scroll{0.0f, 0.0f} {}

void SectionView::onResize(WORD width, WORD height)
{
	captureScroll(RECT{0, 0, width, height});
}

LRESULT SectionView::wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			SolidBrush subtlegrey(D2D1::ColorF(0.08f, 0.08f, 0.08f), mw.rt);

			mw.rt.beginDraw();
			mw.rt.clear(0.1f, 0.1f, 0.1f);
			
			const auto size = mw.getSize();

			for (int i = -scroll.y / noteH; i * noteH + scroll.y < size.bottom; ++i)
			{
				mw.rt.drawBitmap(
					mw.rsc[RSC_TONER],
					Placement{0.0f, i * noteH, 150.0f, 50.0f} + scroll.onlyY(),
					Placement{0.0f, ((256 - i + 10) % 12) * 50.0f, 150.0f, 50.0f}
				);
				mw.rt.drawLine(
					D2D1::Point2F(0.0f, i * noteH) + scroll.onlyY(),
					D2D1::Point2F(size.right, i * noteH) + scroll.onlyY(),
					subtlegrey,
					3.0f
				);
			}

			if (mw.rt.endDraw(mw.d2dfac))
				ValidateRect(mw, nullptr);
		}
		return 0;
		case WM_MOUSEWHEEL:
		{
			scroll.y += GET_WHEEL_DELTA_WPARAM(wParam);
			captureScroll(mw.getSize());
			InvalidateRect(mw, nullptr, FALSE);
		}
		return 0;
	}
	return DefWindowProcW(mw, msg, wParam, lParam);
}