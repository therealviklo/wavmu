#pragma once
#include <vector>
#include "gui.h"

class SectionView : public View
{
private:
	SectionRef& sect;
	Offset scroll;
	/* Av någon anledning så kallade jag det här för "NoteSelect"
	   fast det används när man ska skapa noter, inte välja dem ... */
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
	// ... så det här får heta "markedNotes".
	std::vector<size_t> markedNotes;
	struct NoteMark
	{
		bool marking;
		D2D1_RECT_F reg;

		NoteMark() noexcept :
			marking(false),
			reg{} {}
	} nm;

	void deleteNotes(MainWindow& mw);

	void captureScroll(RECT size);
public:
	SectionView(SectionRef& sect) noexcept;
	
	void onResize(WORD /*width*/, WORD /*height*/) override;

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};