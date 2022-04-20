#include "trackview.h"

namespace
{
	inline Placement channelHeadPlace(size_t n, Offset scroll)
	{
		return Placement{0.0f, 100.0f * n, 200.0f, 100.0f} + scroll.onlyY();
	}

	inline Placement trackPlusPlace(size_t n, Offset scroll)
	{
		const auto head = channelHeadPlace(n, scroll);
		return {head.x + head.w - 30.0f, head.y, 30.0f, 30.0f};
	}

	inline Placement deleteTrackPlace(size_t n, Offset scroll)
	{
		const auto head = channelHeadPlace(n, scroll);
		return {head.x, head.y + head.h - 30.0f, 30.0f, 30.0f};
	}

	inline Placement sectionPlace(const SectionRef& sect, size_t n, float noteSize, float secMoveX, Offset scroll)
	{
		const float sectLen = std::max<float>(sect.section->calcLength(), 1.0f);
		const Placement place = channelHeadPlace(n, scroll);
		return Placement{
			place.x + place.w + std::max<float>(noteSize * (float)sect.timestamp + secMoveX, 0.0),
			place.y,
			sectLen * noteSize,
			place.h
		} + scroll.onlyX();
	}

	inline Placement trianglePlace(size_t n, Offset scroll)
	{
		const auto head = channelHeadPlace(n, scroll);
		const auto plus = trackPlusPlace(n, scroll);
		return {
			plus.x + 8.0f,
			plus.y + plus.h + 5.0f,
			head.x + head.w - 8.0f - (plus.x + 8.0f),
			head.y + head.h - 5.0f - (plus.y + plus.h + 5.0f)
		};
	}

	inline Placement sliderPlace(size_t n, double val, Offset scroll)
	{
		const auto tp = trianglePlace(n, scroll);
		return {
			tp.x,
			static_cast<float>(tp.y + tp.h * (1.0 - val) - 5.0 / 2.0),
			tp.w,
			5.0f	
		};
	}

	inline Placement trackLabelPlace(size_t n, Offset scroll)
	{
		const auto chp = channelHeadPlace(n, scroll);
		const auto pp = trackPlusPlace(n, scroll);
		return {chp.x, chp.y, pp.x - chp.x, pp.h};
	}
}

float TrackView::sectIsSelected(size_t track, size_t sect)
{
	if (selectedSections.contains(track))
	{
		for (const auto& k : selectedSections.at(track))
		{
			if (k == sect)
			{
				return true;
			}
		}
	}
	return false;
}

float TrackView::getLeastSnapDiff(float x, const std::vector<Track>& tracks)
{
	const Placement chp = channelHeadPlace(0, scroll);
	float leastSnap = chp.x + chp.w - x;
	for (size_t i = 0; i < tracks.size(); i++)
	{
		for (size_t j = 0; j < tracks[i].sections.size(); j++)
		{
			if (sectIsSelected(i, j)) continue;
			const auto& sect = tracks[i].sections[j];

			const Placement sectPlace = sectionPlace(sect, i, noteSize, 0.0f, scroll);
			const float startSnap = sectPlace.x - x;
			const float ssabs = std::fabsf(startSnap);
			const float endSnap = sectPlace.x + sectPlace.w - x;
			const float esabs = std::fabsf(endSnap);
			if (esabs < ssabs)
			{
				if (esabs < std::fabsf(leastSnap))
				{
					leastSnap = endSnap;
				}
			}
			else
			{
				if (ssabs < std::fabsf(leastSnap))
				{
					leastSnap = startSnap;
				}
			}
		}
	}
	return leastSnap;
}

float TrackView::getSnapDiffFromSelection(const std::vector<Track>& tracks)
{
	float leastSnap = secMove->left - secMove->right;
	for (const auto& i : selectedSections)
	{
		for (size_t j = 0; j < i.second.size(); j++)
		{
			const auto& sect = tracks[i.first].sections[i.second[j]];
			const Placement sectPlace = sectionPlace(sect, i.first, noteSize, secMove->right - secMove->left, scroll);

			const float startSnap = getLeastSnapDiff(sectPlace.x, tracks);
			const float ssabs = std::fabsf(startSnap);
			const float endSnap = getLeastSnapDiff(sectPlace.x + sectPlace.w, tracks);
			const float esabs = std::fabsf(endSnap);
			if (esabs < ssabs)
			{
				if (esabs < std::fabsf(leastSnap))
				{
					leastSnap = endSnap;
				}
			}
			else
			{
				if (ssabs < std::fabsf(leastSnap))
				{
					leastSnap = startSnap;
				}
			}
		}
	}
	return std::fabsf(leastSnap) < 10.0f ? leastSnap : 0.0f;
}

float TrackView::getSecMoveDiffX() const
{
	return secMove ? secMove->right - secMove->left + snap : 0.0f;
}

void TrackView::captureScroll(const std::vector<Track>& tracks)
{
	scroll.x = std::min<float>(scroll.x, 0.0f);
	const Placement addTrackPlace = channelHeadPlace(tracks.size(), Offset{});
	scroll.y = std::clamp<float>(scroll.y, -(addTrackPlace.y + addTrackPlace.h), 0.0f);
}

TrackView::TrackView(D2DFactory& d2dfac) :
	triangle(d2dfac, [](){
		const auto place = trianglePlace(0, {});
		return std::vector<D2D1_POINT_2F>({
			{0.0f, 0.0f},
			{place.w, 0.0f},
			{place.w / 2.0f, place.h}
		});
	}()),
	selectedTrackPlus(-1),
	addSectionPos(0.0f),
	noteSize(50.0f),
	scroll{0.0f, 0.0f},
	snap(0.0f) {}

LRESULT TrackView::wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			SolidBrush darkgrey(D2D1::ColorF(0.05f, 0.05f, 0.05f), mw.rt);
			SolidBrush darkishgrey(D2D1::ColorF(0.07f, 0.07f, 0.07f), mw.rt);
			SolidBrush grey(D2D1::ColorF(0.1f, 0.1f, 0.1f), mw.rt);
			SolidBrush lightishgrey(D2D1::ColorF(0.13f, 0.13f, 0.13f), mw.rt);
			SolidBrush lightgrey(D2D1::ColorF(0.2f, 0.2f, 0.2f), mw.rt);
			SolidBrush darkblue(D2D1::ColorF(0.0f, 0.0f, 0.2f), mw.rt);
			SolidBrush darkishblue(D2D1::ColorF(0.0f, 0.0f, 0.3f), mw.rt);
			SolidBrush blue(D2D1::ColorF(0.0f, 0.0f, 0.4f), mw.rt);
			SolidBrush seldarkblue(D2D1::ColorF(0.0f, 0.0f, 0.3f), mw.rt);
			SolidBrush selblue(D2D1::ColorF(0.0f, 0.0f, 0.5f, 0.5f), mw.rt);
			SolidBrush subtlewhite(D2D1::ColorF(0.8f, 0.8f, 0.8f), mw.rt);

			const auto size = mw.getSize();

			mw.rt.beginDraw();
			mw.rt.clear(0.05f, 0.05f, 0.05f);

			{
				const std::shared_lock sl(mw.tracks.mtx);
				size_t i = 0;
				// Tracks
				for (; i < mw.tracks.data.size(); i++)
				{
					constexpr float dividerW = 5.0f;

					const auto place = channelHeadPlace(i, scroll);
					// Rita bakgrunden
					mw.rt.drawRectangle(
						D2D1::RectF(place.x + place.w, place.y, size.right, place.y + place.h),
						lightishgrey
					);

					// Rita sektioner
					for (size_t j = 0; j < mw.tracks.data[i].sections.size(); j++)
					{
						const bool isSelected = sectIsSelected(i, j);
						const Placement sectPlace = sectionPlace(
							mw.tracks.data[i].sections[j],
							i,
							noteSize,
							isSelected ? getSecMoveDiffX() : 0.0f,
							scroll
						);
						mw.rt.drawRectangle(
							sectPlace,
							isSelected ? darkishblue : blue
						);
						constexpr float outlineW = 1.0f;
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

					// Rita pluset
					const auto plusPlace = trackPlusPlace(i, scroll);
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

					// Rita triangeln
					const auto tp = trianglePlace(i, scroll);
					mw.rt.drawPolygon(
						triangle,
						D2D1::Point2F(tp.x, tp.y),
						darkishgrey
					);
					// Rita slidern
					const auto sp = sliderPlace(i, mw.tracks.data[i].volume, scroll);
					mw.rt.drawRectangle(sp, lightgrey);

					// Rita soptunnan
					const auto delTrackPlace = deleteTrackPlace(i, scroll);
					mw.rt.drawRectangle(
						delTrackPlace,
						lightishgrey
					);
					mw.rt.drawRectangle(
						D2D1::RectF(
							delTrackPlace.x + delTrackPlace.w / 2.0f - 6.0f,
							delTrackPlace.y + 5.0f,
							delTrackPlace.x + delTrackPlace.w / 2.0f + 6.0f,
							delTrackPlace.y + delTrackPlace.h - 5.0f
						),
						darkgrey
					);
					mw.rt.drawRectangle(
						D2D1::RectF(
							delTrackPlace.x + 6.0f,
							delTrackPlace.y + delTrackPlace.h / 2.0f - 7.0f,
							delTrackPlace.x + delTrackPlace.w - 6.0f,
							delTrackPlace.y + delTrackPlace.h / 2.0f - 3.0f
						),
						darkgrey
					);

					// Rita namnet
					mw.rt.drawText(
						mw.tracks.data[i].instrument->getDispName(),
						mw.font1,
						trackLabelPlace(i, scroll),
						subtlewhite
					);
				}
				// Rita lägg-till-track-knappen
				const auto place = channelHeadPlace(i, scroll);
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

			if (sm.marking)
			{
				mw.rt.drawRectangle(
					sm.reg,
					selblue
				);
				mw.rt.outlineRectangle(
					sm.reg,
					seldarkblue,
					1.0f
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
			if (pressInPlace(mx, my, channelHeadPlace(mw.tracks.data.size(), scroll)))
			{
				// Lägg till instrument
				std::unique_ptr<wchar_t[]> instrName((wchar_t*)displayDialogueBox(createTrack, mw));
				if (instrName)
				{
					std::unique_lock ul(mw.tracks.mtx, std::defer_lock);
					if (ul.try_lock())
					{
						mw.tracks.data.emplace_back(
							Track{
								createInstrument(wstringToString(instrName.get()).c_str()),
								std::vector<SectionRef>{}
							}
						);
						InvalidateRect(mw, nullptr, FALSE);
					}
				}
				return 0;
			}
			{
				std::shared_lock sl(mw.tracks.mtx);
				for (size_t i = 0; i < mw.tracks.data.size(); i++)
				{
					const auto headPlace = channelHeadPlace(i, scroll);
					if (pressInPlace(mx, my, trackPlusPlace(i, scroll)))
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
						selectedSections.clear();
						InvalidateRect(mw, nullptr, FALSE);
						return 0;
					}
					const auto tp = trianglePlace(i, scroll);
					if (pressInPlace(mx, my, tp))
					{
						// Har tryckt på slidern
						sliderDrag.emplace(SliderDrag{static_cast<float>(my), i});
						ExclusiveLockGuard elg(sl);
						if (elg.try_lock())
						{
							auto& vol = mw.tracks.data[i].volume;
							vol = std::clamp<double>(
								(tp.y + tp.h - my) / tp.h,
								0.0,
								1.0
							);
						}
						InvalidateRect(mw, nullptr, FALSE);
						return 0;
					}
					if (pressInPlace(mx, my, deleteTrackPlace(i, scroll)))
					{
						// Har tryckt på en soptunna
						ExclusiveLockGuard elg(sl);
						if (!elg.try_lock())
							return 0;
						if (MessageBoxW(
								nullptr,
								L"Är du säker på att du vill ta bort spåret?",
								L"Ta bort spår",
								MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
							) == IDYES)
						{
							mw.tracks.data.erase(mw.tracks.data.begin() + i);
							InvalidateRect(mw, nullptr, FALSE);
						}
						return 0;
					}
					if (selectedTrackPlus == -1)
					{
						for (size_t j = 0; j < mw.tracks.data[i].sections.size(); j++)
						{
							const auto sectPlace =
								sectionPlace(
									mw.tracks.data[i].sections[j],
									i,
									noteSize,
									0.0f,
									scroll
								);
							if (pressInPlace(mx, my, sectPlace) && sectIsSelected(i, j))
							{
								// Flytta markering
								secMove.emplace(
									D2D1_RECT_F{
										static_cast<float>(mx),
										static_cast<float>(my),
										static_cast<float>(mx),
										static_cast<float>(my)
									}
								);
								snap = 0.0f;
								InvalidateRect(mw, nullptr, FALSE);
								return 0;
							}
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
								std::make_shared<Section>(Section{{}}),
								(mx - (headPlace.x + headPlace.w) - scroll.x) / noteSize
							});
							selectedTrackPlus = -1;
							InvalidateRect(mw, nullptr, FALSE);
							return 0;
						}
					}
				}
				if (selectedTrackPlus == -1)
				{
					// Välj sektion
					sm.marking = true;
					sm.reg.left = mx;
					sm.reg.top = my;
					sm.reg.right = mx;
					sm.reg.bottom = my;
					InvalidateRect(mw, nullptr, FALSE);
					return 0;
				}
			}
		}
		break;
		case WM_LBUTTONUP:
		{
			const int mx = GET_X_LPARAM(lParam);
			const int my = GET_Y_LPARAM(lParam);
			if (sm.marking)
			{
				sm.marking = false;
				sm.reg.right = mx;
				sm.reg.bottom = my;

				if (sm.reg.right < sm.reg.left)
				{
					std::swap(sm.reg.right, sm.reg.left);
				}
				if (sm.reg.bottom < sm.reg.top)
				{
					std::swap(sm.reg.bottom, sm.reg.top);
				}

				selectedSections.clear();
				if (sm.reg.right == sm.reg.left && sm.reg.bottom == sm.reg.top)
				{
					const std::shared_lock sl(mw.tracks.mtx);
					for (size_t i = 0; i < mw.tracks.data.size(); i++)
					{
						for (size_t j = 0; j < mw.tracks.data[i].sections.size(); j++)
						{
							const auto secPlace =
								sectionPlace(
									mw.tracks.data[i].sections[j],
									i,
									noteSize,
									0.0f,
									scroll
								);
							if (rectsColliding(secPlace, sm.reg))
							{
								selectedSections.clear();
								selectedSections.emplace(i, std::vector<size_t>{j});
							}
						}
					}
				}
				else
				{
					const std::shared_lock sl(mw.tracks.mtx);
					for (size_t i = 0; i < mw.tracks.data.size(); i++)
					{
						std::vector<size_t> selSectsForTrack;
						for (size_t j = 0; j < mw.tracks.data[i].sections.size(); j++)
						{
							const auto secPlace =
								sectionPlace(
									mw.tracks.data[i].sections[j],
									i,
									noteSize,
									0.0f,
									scroll
								);
							if (rectsColliding(secPlace, sm.reg))
							{
								selSectsForTrack.push_back(j);
							}
						}
						selectedSections.emplace(i, std::move(selSectsForTrack));
					}
				}
				InvalidateRect(mw, nullptr, FALSE);
				return 0;
			}
			if (secMove)
			{
				secMove->right = mx;
				secMove->bottom = my;
				const float diffX = getSecMoveDiffX();
				std::unique_lock ul(mw.tracks.mtx, std::defer_lock);
				if (ul.try_lock())
				{
					for (const auto& i : selectedSections)
					{
						for (const auto& j : i.second)
						{
							auto& ts = mw.tracks.data[i.first].sections[j].timestamp;
							ts = std::max<double>(0.0, ts + diffX / noteSize);
						}
					}
				}

				secMove.reset();
				InvalidateRect(mw, nullptr, FALSE);
				return 0;
			}
			if (sliderDrag)
			{
				std::unique_lock ul(mw.tracks.mtx, std::defer_lock);
				if (ul.try_lock())
				{
					const auto tp = trianglePlace(sliderDrag->track, scroll);
					auto& vol = mw.tracks.data[sliderDrag->track].volume;
					vol =
						std::clamp<double>(
							(tp.y + tp.h - my) / tp.h,
							0.0,
							1.0
						);
				}

				sliderDrag.reset();
				InvalidateRect(mw, nullptr, FALSE);
				return 0;
			}
		}
		break;
		case WM_LBUTTONDBLCLK:
		{
			const int mx = GET_X_LPARAM(lParam);
			const int my = GET_Y_LPARAM(lParam);
			const std::shared_lock sl(mw.tracks.mtx);
			for (size_t i = 0; i < mw.tracks.data.size(); i++)
			{
				for (auto& secRef : mw.tracks.data[i].sections)
				{
					const auto secPlace = sectionPlace(secRef, i, noteSize, 0.0f, scroll);
					if (pressInPlace(mx, my, secPlace))
					{
						selectedSections.clear();
						mw.vpush(std::make_unique<SectionView>(secRef));
						return 0;
					}
				}
			}
		}
		break;
		case WM_MOUSEMOVE:
		{
			const int mx = GET_X_LPARAM(lParam);
			const int my = GET_Y_LPARAM(lParam);
			if (selectedTrackPlus != -1)
			{
				size_t numTracks;
				{
					const std::shared_lock sl(mw.tracks.mtx);
					numTracks = mw.tracks.data.size();
				}
				long long newSelectedTrackPlus = 0;
				for (size_t i = 0; i < numTracks; i++)
				{
					const auto headPlace = channelHeadPlace(i, scroll);
					if (my >= headPlace.y)
					{
						newSelectedTrackPlus = i;
					}
					else break;
				}
				selectedTrackPlus = newSelectedTrackPlus;

				const auto headPlace = channelHeadPlace(selectedTrackPlus, scroll);
				addSectionPos = std::max<float>(mx - (headPlace.x + headPlace.w), 0.0f);
				InvalidateRect(mw, nullptr, FALSE);
			}
			if (sm.marking)
			{
				sm.reg.right = mx;
				sm.reg.bottom = my;
				InvalidateRect(mw, nullptr, FALSE);
			}
			if (secMove)
			{
				secMove->right = mx;
				secMove->bottom = my;
				snap = getSnapDiffFromSelection(mw.tracks.data);
				InvalidateRect(mw, nullptr, FALSE);
			}
			if (sliderDrag)
			{
				std::unique_lock ul(mw.tracks.mtx, std::defer_lock);
				if (ul.try_lock())
				{
					const auto tp = trianglePlace(sliderDrag->track, scroll);
					auto& vol = mw.tracks.data[sliderDrag->track].volume;
					vol =
						std::clamp<double>(
							(tp.y + tp.h - my) / tp.h,
							0.0,
							1.0
						);
				}
				InvalidateRect(mw, nullptr, FALSE);
			}
		}
		return 0;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_SPACE:
				{
					if (mw.player.playing())
					{
						mw.player.stop();
					}
					else
					{
						mw.player.start(mw.tracks, 240);
					}
				}
				return 0;
				case VK_DELETE:
				{
					std::unique_lock ul(mw.tracks.mtx, std::defer_lock);
					if (!ul.try_lock())
						return 0;
					for (auto& i : selectedSections)
					{
						std::sort(
							i.second.begin(),
							i.second.end(),
							std::greater<size_t>()
						);
						for (const auto& j : i.second)
						{
							mw.tracks.data[i.first].sections.erase(
								mw.tracks.data[i.first].sections.begin() + j
							);
						}
					}
					selectedSections.clear();
					InvalidateRect(mw, nullptr, FALSE);
				}
				return 0;
			}
		}
		break;
		case WM_MOUSEWHEEL:
		{
			const std::shared_lock sl(mw.tracks.mtx);
			if (LOWORD(wParam) & MK_SHIFT)
				scroll.x += GET_WHEEL_DELTA_WPARAM(wParam);
			else
				scroll.y += GET_WHEEL_DELTA_WPARAM(wParam);
			captureScroll(mw.tracks.data);
			InvalidateRect(mw, nullptr, FALSE);
		}
		return 0;
		case WM_MOUSEHWHEEL:
		{
			const std::shared_lock sl(mw.tracks.mtx);
			scroll.x -= GET_WHEEL_DELTA_WPARAM(wParam);
			captureScroll(mw.tracks.data);
			InvalidateRect(mw, nullptr, FALSE);
		}
		return 0;
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == 1)
			{
				switch (LOWORD(wParam))
				{
					case ACC_COPY:
					{
						bool sectAdded = false;
						std::map<size_t, std::vector<SectionRef>> newClipboard;
						const std::shared_lock sl(mw.tracks.mtx);
						for (const auto& i : selectedSections)
						{
							std::vector<SectionRef> sects;
							for (const auto& j : i.second)
							{
								sects.push_back(mw.tracks.data[i.first].sections[j]);
							}
							sectAdded = sectAdded || !sects.empty();
							newClipboard.emplace(i.first, std::move(sects));
						}
						if (sectAdded)
							clipboard = newClipboard;
					}
					return 0;
					case ACC_PASTE:
					{
						std::unique_lock ul(mw.tracks.mtx, std::defer_lock);
						if (!ul.try_lock())
							return 0;
						selectedSections.clear();
						for (const auto& i : clipboard)
						{
							std::vector<size_t> indices;
							for (const auto& j : i.second)
							{
								indices.push_back(mw.tracks.data[i.first].sections.size());
								mw.tracks.data[i.first].sections.push_back(j);
							}
							selectedSections.emplace(i.first, std::move(indices));
						}
						InvalidateRect(mw, nullptr, FALSE);
					}
					return 0;
				}
			}
		}
		break;
	}
	return DefWindowProcW(mw, msg, wParam, lParam);
}