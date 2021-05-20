#pragma once
#include "gui.h"

class SectionView : public View
{
private:
	SectionRef& sect;
	Offset scroll;
	struct NoteSelect
	{
		bool selecting;
		Tone tone;
		double start;
		double end;

		NoteSelect() noexcept;

		static Tone getTone(float y, Offset scroll);
		static double getTime(float x, Offset scroll, TimeSignature timesig);
	} ns;

	void captureScroll(RECT size);
public:
	SectionView(SectionRef& sect) noexcept;
	
	void onResize(WORD /*width*/, WORD /*height*/) override;

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};