#pragma once
#include "gui.h"

class SectionView : public View
{
private:
	Section& sect;
	Offset scroll;

	void captureScroll(RECT size);
public:
	SectionView(Section& sect) noexcept;
	
	void onResize(WORD /*width*/, WORD /*height*/) override;

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};