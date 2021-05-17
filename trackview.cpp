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

	inline Placement sectionPlace(const SectionRef& sect, size_t n, float noteSize)
	{
		const float sectLen = std::max<float>(sect.section->calcLength(), 1.0f);
		const Placement place = channelHeadPlace(n);
		return Placement{place.x + place.w + noteSize * (float)sect.timestamp, place.y, sectLen * noteSize, place.h};
	}
}

TrackView::TrackView() :
	selectedTrackPlus(-1),
	addSectionPos(0.0f),
	noteSize(50.0f) {}

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
			SolidBrush darkblue(D2D1::ColorF(0.0f, 0.0f, 0.2f), mw.rt);
			SolidBrush darkishblue(D2D1::ColorF(0.0f, 0.0f, 0.3f), mw.rt);
			SolidBrush blue(D2D1::ColorF(0.0f, 0.0f, 0.4f), mw.rt);

			const auto size = mw.getSize();

			mw.rt.beginDraw();
			mw.rt.clear(0.05f, 0.05f, 0.05f);

			{
				const std::lock_guard lg(mw.tracks.mtx);
				size_t i = 0;
				// Tracks
				for (; i < mw.tracks.data.size(); i++)
				{
					constexpr float dividerW = 5.0f;

					const auto place = channelHeadPlace(i);
					// Rita bakgrunden
					mw.rt.drawRectangle(
						D2D1::RectF(place.x + place.w, place.y, size.right, place.y + place.h),
						lightishgrey
					);

					// Rita sektioner
					for (const auto& sect : mw.tracks.data[i].sections)
					{
						const Placement sectPlace = sectionPlace(sect, i, noteSize);
						mw.rt.drawRectangle(
							sectPlace,
							selectedSection == &sect ? darkishblue : blue
						);
						constexpr float outlineW = 2.0f;
						mw.rt.outlineRectangle(
							D2D1::RectF(
								sectPlace.x + outlineW / 2.0f,
								sectPlace.y + outlineW / 2.0f,
								sectPlace.x + sectPlace.w - outlineW / 2.0f,
								sectPlace.y + sectPlace.h - dividerW - outlineW / 2.0f
							),
							darkblue,
							outlineW
						);
					}

					// Rita eventuellt förhandsvisningen av sektionen
					if (i == (size_t)selectedTrackPlus)
					{
						mw.rt.drawRectangle(
							Placement{place.x + place.w + addSectionPos, place.y, noteSize, place.h},
							darkblue
						);
					}

					// Rita "huvudet" till vänster och separatorlinjen
					mw.rt.drawRectangle(
						place,
						grey
					);
					mw.rt.drawLine(
						D2D1::Point2F(0.0f, place.y + place.h - dividerW / 2.0f),
						D2D1::Point2F(size.right, place.y + place.h - dividerW / 2.0f),
						grey,
						dividerW
					);

					// Rita plusset
					const auto plusPlace = trackPlusPlace(i);
					if (i == (size_t)selectedTrackPlus)
					{
						// Valda bakgrunden
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
				// Rita lägg-till-track-knappen
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
			const auto size = mw.getSize();
			if (pressInPlace(mx, my, channelHeadPlace(mw.tracks.data.size())))
			{
				// Lägg till instrument
				const std::lock_guard lg(mw.tracks.mtx);
				if (!mw.tracks.playing.test(std::memory_order::relaxed))
				{
					mw.tracks.data.emplace_back(Track{std::make_unique<SinInstrument>(), std::vector<SectionRef>{}});
					InvalidateRect(mw, nullptr, FALSE);
				}
				return 0;
			}
			{
				selectedSection = nullptr;
				const std::lock_guard lg(mw.tracks.mtx);
				for (size_t i = 0; i < mw.tracks.data.size(); i++)
				{
					const auto headPlace = channelHeadPlace(i);
					if (pressInPlace(mx, my, trackPlusPlace(i)))
					{
						// Har tryckt på ett plus
						if (i == (size_t)selectedTrackPlus)
						{
							selectedTrackPlus = -1;
						}
						else
						{
							selectedTrackPlus = i;
						}
						addSectionPos = std::max<float>(mx - (headPlace.x + headPlace.w), 0.0f);
						InvalidateRect(mw, nullptr, FALSE);
						return 0;
					}
					if (selectedTrackPlus == -1)
					{
						// Välj sektion
						if (pressInPlace( // Kollar om man ens har tryckt på spåret
							mx,
							my,
							Placement{
								headPlace.x + headPlace.w,
								headPlace.y,
								size.right - headPlace.x - headPlace.w,
								headPlace.h
							}
						))
						{
							// Går igenom spåren och kollar om man har tryckt på det
							for (auto it = mw.tracks.data[i].sections.rbegin();
								it != mw.tracks.data[i].sections.rend();
								++it)
							{
								const Placement sectPlace = sectionPlace(*it, i, noteSize);
								if (pressInPlace(
									mx,
									my,
									sectPlace
								))
								{
									selectedSection = &*it;
									break;
								}
							}
							InvalidateRect(mw, nullptr, FALSE);
							return 0;
						}
					}
					else
					{
						// Kollar om man ska lägga till en sektion
						if (i == (size_t)selectedTrackPlus && pressInPlace(
								mx,
								my,
								Placement{
									headPlace.x + headPlace.w,
									headPlace.y,
									size.right - headPlace.x - headPlace.w,
									headPlace.h
								}
							))
						{
							mw.tracks.data[i].sections.emplace_back(SectionRef{
								std::make_shared<Section>(Section{{
									{0.0, 1/8., 60},
									{1/8., 1/8., 60},
									{2/8., 2/8., 72},
									{4/8., 3/8., 67},
									{7/8., 2/8., 66},
									
									{9/8., 2/8., 65},
									{11/8., 2/8., 63},
									{13/8., 1/8., 60},
									{14/8., 1/8., 63},
									{15/8., 1/8., 65},
									

									{0.0 + 1 * 2.0, 1/8., 58},
									{1/8. + 1 * 2.0, 1/8., 58},
									{2/8. + 1 * 2.0, 2/8., 72},
									{4/8. + 1 * 2.0, 3/8., 67},
									{7/8. + 1 * 2.0, 2/8., 66},
									
									{9/8. + 1 * 2.0, 2/8., 65},
									{11/8. + 1 * 2.0, 2/8., 63},
									{13/8. + 1 * 2.0, 1/8., 60},
									{14/8. + 1 * 2.0, 1/8., 63},
									{15/8. + 1 * 2.0, 1/8., 65},
									

									{0.0 + 2 * 2.0, 1/8., 57},
									{1/8. + 2 * 2.0, 1/8., 57},
									{2/8. + 2 * 2.0, 2/8., 72},
									{4/8. + 2 * 2.0, 3/8., 67},
									{7/8. + 2 * 2.0, 2/8., 66},
									
									{9/8. + 2 * 2.0, 2/8., 65},
									{11/8. + 2 * 2.0, 2/8., 63},
									{13/8. + 2 * 2.0, 1/8., 60},
									{14/8. + 2 * 2.0, 1/8., 63},
									{15/8. + 2 * 2.0, 1/8., 65},
									

									{0.0 + 3 * 2.0, 1/8., 56},
									{1/8. + 3 * 2.0, 1/8., 56},
									{2/8. + 3 * 2.0, 2/8., 72},
									{4/8. + 3 * 2.0, 3/8., 67},
									{7/8. + 3 * 2.0, 2/8., 66},
									
									{9/8. + 3 * 2.0, 2/8., 65},
									{11/8. + 3 * 2.0, 2/8., 63},
									{13/8. + 3 * 2.0, 1/8., 60},
									{14/8. + 3 * 2.0, 1/8., 63},
									{15/8. + 3 * 2.0, 1/8., 65},
								}}),
								(mx - (headPlace.x + headPlace.w)) / noteSize
							});
							selectedTrackPlus = -1;
							InvalidateRect(mw, nullptr, FALSE);
							return 0;
						}
					}
				}
			}
		}
		break;
		case WM_LBUTTONDBLCLK:
		{
			if (selectedSection)
			{
				mw.vpush(std::make_unique<SectionView>(*selectedSection->section));
				return 0;
			}
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (selectedTrackPlus != -1)
			{
				const auto headPlace = channelHeadPlace(selectedTrackPlus);
				addSectionPos = std::max<float>(GET_X_LPARAM(lParam) - (headPlace.x + headPlace.w), 0.0f);
				InvalidateRect(mw, nullptr, FALSE);
			}
		}
		return 0;
	}
	return DefWindowProcW(mw, msg, wParam, lParam);
}