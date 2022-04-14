#include "sectionview.h"

constexpr float noteH = 17.0f;
constexpr float noteW = 8.0f * noteH;

namespace
{
	inline Placement getNotePos(const Note& note, Offset scroll)
	{
		return Placement{
				(float)note.timestamp * noteW,
				(255 - note.tone) * noteH,
				(float)note.length * noteW,
				noteH
			} + scroll;
	}
}

SectionView::NoteSelect::NoteSelect() noexcept :
	selecting(false),
	tone(0),
	start(0.0),
	end(0.0) {}
	
Tone SectionView::NoteSelect::getTone(float y, Offset scroll)
{
	return 256 - (y - scroll.y) / noteH;
}

double SectionView::NoteSelect::getTime(float x, Offset scroll, TimeSignature timesig, int zoom)
{
	const float qnoteW = noteW / (timesig.btm * std::exp2f(zoom));
	return std::roundf((x - scroll.x) / qnoteW) * qnoteW / noteW;
}

void SectionView::deleteNotes(MainWindow& mw)
{
	const std::lock_guard lg(mw.tracks.mtx);
	if (!mw.tracks.playing.test(std::memory_order_relaxed))
	{
		std::sort(markedNotes.begin(), markedNotes.end(), std::greater<size_t>());
		for (const auto& i : markedNotes)
		{
			sect.section->notes.erase(sect.section->notes.begin() + i);
		}
		markedNotes.clear();
	}
}

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
	scroll.x = std::min<float>(0.0f, scroll.x);
}

SectionView::SectionView(SectionRef& sect) noexcept :
	sect(sect),
	scroll{0.0f, -(255 - 124) * noteH},
	raster(0) {}

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
			const std::lock_guard lg(mw.tracks.mtx);

			SolidBrush subtlegrey(D2D1::ColorF(0.08f, 0.08f, 0.08f), mw.rt);
			SolidBrush subtlessgrey(D2D1::ColorF(0.06f, 0.06f, 0.06f), mw.rt);
			SolidBrush darkblue(D2D1::ColorF(0.0f, 0.0f, 0.2f), mw.rt);
			SolidBrush blue(D2D1::ColorF(0.0f, 0.0f, 0.4f), mw.rt);
			SolidBrush markedblue(D2D1::ColorF(0.0f, 0.0f, 0.3f), mw.rt);
			SolidBrush seldarkblue(D2D1::ColorF(0.0f, 0.0f, 0.3f), mw.rt);
			SolidBrush selblue(D2D1::ColorF(0.0f, 0.0f, 0.5f, 0.5f), mw.rt);

			mw.rt.beginDraw();
			mw.rt.clear(0.1f, 0.1f, 0.1f);
			
			const auto size = mw.getSize();

			const float lineInterval = noteW / (sect.section->timesig.btm * std::exp2f(raster));
			for (int i = -scroll.x / lineInterval; i * lineInterval + scroll.x < size.right; ++i)
			{
				auto iTimesExp2 = [](int i, int e) -> int {
					if (e < 0)
					{
						return i >> -e;
					}
					else
					{
						return i << e;
					}
				};
				auto safeMod = [](int a, int b) -> int {
					if (!b) return 0;
					return a % b;
				};
				const bool measureLine = safeMod(i, iTimesExp2(sect.section->timesig.top, raster)) == 0;
				mw.rt.drawLine(
					D2D1::Point2F(i * lineInterval, 0.0f) + scroll.onlyX(),
					D2D1::Point2F(i * lineInterval, size.bottom) + scroll.onlyX(),
					measureLine ? subtlessgrey : subtlegrey,
					measureLine ? 3.0f : 1.0f
				);
			}
			for (int i = -scroll.y / noteH; i * noteH + scroll.y < size.bottom; ++i)
			{
				const int tone = (255 - i + 11) % 12;
				mw.rt.drawBitmap(
					mw.rsc[RSC_TONER],
					Placement{0.0f, i * noteH, 150.0f * noteH / 50.0f, 50.0f * noteH / 50.0f} + scroll.onlyY(),
					Placement{0.0f, tone * 50.0f, 150.0f, 50.0f}
				);

				if (((255 - i) - 124) % 12 == 0)
				{
					int oct = ((255 - i) - 124) / 12 + 4;
					float off = noteH * 0.4f;
					if (oct < 0)
					{
						mw.rt.drawBitmap(
							mw.rsc[RSC_TONER],
							Placement{off, i * noteH, 50.0f * noteH / 50.0f, 50.0f * noteH / 50.0f} + scroll.onlyY(),
							Placement{150.0f, 10 * 50.0f, 50.0f, 50.0f}
						);
						oct = -oct;
						off += noteH * 0.5f;
					}
					// do
					// {
					// 	mw.rt.drawBitmap(
					// 		mw.rsc[RSC_TONER],
					// 		Placement{off, i * noteH, 50.0f * noteH / 50.0f, 50.0f * noteH / 50.0f} + scroll.onlyY(),
					// 		Placement{150.0f, (oct % 10) * 50.0f, 50.0f, 50.0f}
					// 	);
					// 	oct /= 10;
					// 	off += noteH * 0.5f;
					// } while (oct);
					auto drawOct = [this, &mw, &i, &off](auto& rec, int oct) -> void {
						if (oct / 10) rec(rec, oct / 10);
						mw.rt.drawBitmap(
							mw.rsc[RSC_TONER],
							Placement{off, i * noteH, 50.0f * noteH / 50.0f, 50.0f * noteH / 50.0f} + scroll.onlyY(),
							Placement{150.0f, (oct % 10) * 50.0f, 50.0f, 50.0f}
						);
						off += noteH * 0.5f;
					};
					drawOct(drawOct, oct);
				}

				mw.rt.drawLine(
					D2D1::Point2F(0.0f, i * noteH) + scroll.onlyY(),
					D2D1::Point2F(size.right, i * noteH) + scroll.onlyY(),
					subtlegrey,
					3.0f
				);
			}

			auto drawNote = [&](const Placement& r, bool marked = false) -> void {
				mw.rt.drawRectangle(
					r,
					marked ? markedblue : blue
				);
				mw.rt.outlineRectangle(
					r,
					darkblue,
					1.0f
				);
			};
			for (size_t i = 0; i < sect.section->notes.size(); ++i)
			{
				drawNote(
					getNotePos(sect.section->notes[i], scroll),
					std::find(
						markedNotes.begin(),
						markedNotes.end(),
						i
					) != markedNotes.end()
				);
			}
			if (ns.selecting)
			{
				drawNote(getNotePos(Note{ns.start, ns.end - ns.start, ns.tone}, scroll));
			}

			if (nm.marking)
			{
				mw.rt.drawRectangle(
					nm.reg,
					selblue
				);
				mw.rt.outlineRectangle(
					nm.reg,
					seldarkblue,
					1.0f
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
		case WM_MOUSEHWHEEL:
		{
			scroll.x -= GET_WHEEL_DELTA_WPARAM(wParam);
			captureScroll(mw.getSize());
			InvalidateRect(mw, nullptr, FALSE);
		}
		return 0;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_SPACE:
				{
					auto checkIfPlaying = [&mw]() -> bool {
						const std::lock_guard lg(mw.tracks.mtx);
						return mw.tracks.playing.test(std::memory_order_relaxed);
					};
					if (checkIfPlaying())
					{
						mw.player.stop();
					}
					else
					{
						const double timestamp = [&]() -> double {
							const std::lock_guard lg(mw.tracks.mtx);
							return sect.timestamp;
						}();
						mw.player.start(mw.tracks, 240, timestamp);
					}
				}
				return 0;
				case VK_ESCAPE:
				{
					mw.vpop();
				}
				return 0;
				case VK_DELETE:
				{
					deleteNotes(mw);
					InvalidateRect(mw, nullptr, FALSE);
				}
				return 0;
				case VK_OEM_PLUS:
				{
					raster += 1;
					InvalidateRect(mw, nullptr, FALSE);
				}
				return 0;
				case VK_OEM_MINUS:
				{
					if (raster > -std::log2(sect.section->timesig.btm))
					{
						raster -= 1;
						InvalidateRect(mw, nullptr, FALSE);
					}
				}
				return 0;
			}
		}
		break;
		case WM_RBUTTONDOWN:
		{
			const std::lock_guard lg(mw.tracks.mtx);
			const int x = GET_X_LPARAM(lParam); 
			const int y = GET_Y_LPARAM(lParam); 
			ns.tone = ns.getTone(y, scroll);
			ns.start = ns.getTime(x, scroll, sect.section->timesig, raster);
			ns.end = ns.start;
			ns.selecting = true;
			InvalidateRect(mw, nullptr, FALSE);
		}
		return 0;
		case WM_RBUTTONUP:
		{
			if (ns.selecting)
			{
				ns.selecting = false;
				const std::lock_guard lg(mw.tracks.mtx);
				if (!mw.tracks.playing.test(std::memory_order_relaxed))
				{
					if (std::abs(ns.end - ns.start) >= 0.5 / sect.section->timesig.btm)
					{
						if (ns.end < ns.start)
						{
							sect.section->notes.emplace_back(Note{ns.end, ns.start - ns.end, ns.tone});
						}
						else
						{
							sect.section->notes.emplace_back(Note{ns.start, ns.end - ns.start, ns.tone});
						}
					}
				}
				InvalidateRect(mw, nullptr, FALSE);
			}
		}
		return 0;
		case WM_LBUTTONDOWN:
		{
			const int x = GET_X_LPARAM(lParam);
			const int y = GET_Y_LPARAM(lParam);
			nm.marking = true;
			nm.reg.left = x;
			nm.reg.top = y;
			nm.reg.right = x;
			nm.reg.bottom = y;
			InvalidateRect(mw, nullptr, FALSE);
		}
		return 0;
		case WM_LBUTTONUP:
		{
			if (nm.marking)
			{
				const int x = GET_X_LPARAM(lParam);
				const int y = GET_Y_LPARAM(lParam);
				nm.marking = false;
				nm.reg.right = x;
				nm.reg.bottom = y;

				if (nm.reg.right < nm.reg.left)
				{
					std::swap(nm.reg.right, nm.reg.left);
				}
				if (nm.reg.bottom < nm.reg.top)
				{
					std::swap(nm.reg.bottom, nm.reg.top);
				}

				markedNotes.clear();
				const std::lock_guard lg(mw.tracks.mtx);
				for (size_t i = 0; i < sect.section->notes.size(); ++i)
				{
					if (rectsColliding(nm.reg, getNotePos(sect.section->notes[i], scroll)))
					{
						markedNotes.push_back(i);
					}
				}
				InvalidateRect(mw, nullptr, FALSE);
				return 0;
			}
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (ns.selecting)
			{
				const std::lock_guard lg(mw.tracks.mtx);
				const int x = GET_X_LPARAM(lParam);
				ns.end = ns.getTime(x, scroll, sect.section->timesig, raster);
				InvalidateRect(mw, nullptr, FALSE);
				return 0;
			}
			if (nm.marking)
			{
				const int x = GET_X_LPARAM(lParam);
				const int y = GET_Y_LPARAM(lParam);
				nm.reg.right = x;
				nm.reg.bottom = y;
				InvalidateRect(mw, nullptr, FALSE);
				return 0;
			}
		}
		break;
	}
	return DefWindowProcW(mw, msg, wParam, lParam);
}