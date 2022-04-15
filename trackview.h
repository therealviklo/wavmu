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

	Offset scroll;

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
	float snap;

	std::map<size_t, std::vector<SectionRef>> clipboard;

	float sectIsSelected(size_t track, size_t sect);
	float getLeastSnapDiff(float x, const std::vector<Track>& tracks);
	float getSnapDiffFromSelection(const std::vector<Track>& tracks);
	float getSecMoveDiffX() const;

	void captureScroll(const std::vector<Track>& tracks);
public:
	TrackView();

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};