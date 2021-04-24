#pragma once
#include <algorithm>
#include "gui.h"

class TrackView : public View
{
private:
	long long selectedTrackPlus;
	float addSectionPos;
	float noteSize;
public:
	TrackView();

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};