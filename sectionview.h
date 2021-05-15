#pragma once
#include "gui.h"

class SectionView : public View
{
private:
	Section& sect;
public:
	SectionView(Section& sect) noexcept;
	
	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};