#pragma once
#include <algorithm>
#include "gui.h"
#include "sectionview.h"
#include "sininstrument.h"
#include "createtrack.h"
#include "createinstrument.h"

class TrackView : public View
{
private:
	long long selectedTrackPlus;
	float addSectionPos;
	float noteSize;

	std::map<size_t, std::vector<size_t>> selectedSections;
	struct SecMark
	{
		bool marking;
		D2D1_RECT_F reg;

		SecMark() noexcept :
			marking(false),
			reg{} {}
	} sm;

	std::optional<D2D1_RECT_F> secMove;
	float getSecMoveDiffX() const;
public:
	TrackView();

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};