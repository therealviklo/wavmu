#include "sectionview.h"

SectionView::SectionView(Section& sect) noexcept :
	sect(sect) {}

LRESULT SectionView::wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProcW(mw, msg, wParam, lParam);
}