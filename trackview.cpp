#include "trackview.h"

namespace
{
	inline Placement channelHeadPlace(size_t n)
	{
		return {0.0f, 100.0f * n, 200.0f, 100.0f};
	}

	inline Placement trackPlusPlace(size_t n)
	{
		const auto head = channelHeadPlace(n);
		return {head.x + head.w - 30.0f, head.y, 30.0f, 30.0f};
	}
}

TrackView::TrackView() :
	selectedTrackPlus(-1),
	addSectionX(0.0f) {}

LRESULT TrackView::wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			SolidBrush darkgrey(D2D1::ColorF(0.05f, 0.05f, 0.05f), mw.rt);
			SolidBrush grey(D2D1::ColorF(0.1f, 0.1f, 0.1f), mw.rt);
			SolidBrush lightishgrey(D2D1::ColorF(0.13f, 0.13f, 0.13f), mw.rt);
			SolidBrush lightgrey(D2D1::ColorF(0.2f, 0.2f, 0.2f), mw.rt);
			SolidBrush transblue(D2D1::ColorF(0.0f, 0.0f, 0.4f, 0.5f), mw.rt);
			SolidBrush blue(D2D1::ColorF(0.0f, 0.0f, 0.4f), mw.rt);

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
						D2D1::RectF(place.x + place.w, place.y, size.right, place.y + place.h),
						lightishgrey
					);

					if (i == (size_t)selectedTrackPlus)
					{
						mw.rt.drawRectangle(
							Placement{addSectionX, place.y, 200.0f, place.h},
							transblue
						);
					}

					mw.rt.drawRectangle(
						place,
						grey
					);
					constexpr float dividerW = 5.0f;
					mw.rt.drawLine(
						D2D1::Point2F(0.0f, place.y + place.h - dividerW / 2.0f),
						D2D1::Point2F(size.right, place.y + place.h - dividerW / 2.0f),
						grey,
						dividerW
					);

					const auto plusPlace = trackPlusPlace(i);
					if (i == (size_t)selectedTrackPlus)
					{
						mw.rt.drawRectangle(
							plusPlace,
							lightgrey
						);
					}
					mw.rt.drawRectangle(
						D2D1::RectF(
							plusPlace.x + plusPlace.w / 2.0f - 3.0f,
							plusPlace.y + 5.0f,
							plusPlace.x + plusPlace.w / 2.0f + 3.0f,
							plusPlace.y + plusPlace.h - 5.0f
						),
						darkgrey
					);
					mw.rt.drawRectangle(
						D2D1::RectF(
							plusPlace.x + 5.0f,
							plusPlace.y + plusPlace.h / 2.0f - 3.0f,
							plusPlace.x + plusPlace.w - 5.0f,
							plusPlace.y + plusPlace.h / 2.0f + 3.0f
						),
						darkgrey
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
					mw.tracks.data.emplace_back(Track{std::make_unique<SinInstrument>(), std::vector<Track::SectionRef>{}});
					InvalidateRect(mw, nullptr, FALSE);
				}
				return 0;
			}
			{
				const std::lock_guard lg(mw.tracks.mtx);
				for (size_t i = 0; i < mw.tracks.data.size(); i++)
				{
					if (pressInPlace(mx, my, trackPlusPlace(i)))
					{
						if (i == (size_t)selectedTrackPlus)
						{
							selectedTrackPlus = -1;
						}
						else
						{
							selectedTrackPlus = i;
						}
						addSectionX = mx;
						InvalidateRect(mw, nullptr, FALSE);
						return 0;
					}
				}
			}
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (selectedTrackPlus != -1)
			{
				addSectionX = GET_X_LPARAM(lParam);
				InvalidateRect(mw, nullptr, FALSE);
			}
		}
		return 0;
	}
	return DefWindowProcW(mw, msg, wParam, lParam);
}