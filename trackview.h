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
	SectionRef* selectedSection;
public:
	TrackView();

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};