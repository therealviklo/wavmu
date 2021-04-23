#pragma once
#include "gui.h"

class TrackView : public View
{
private:
	long long selectedTrackPlus;
	float addSectionX;
public:
	TrackView();

	LRESULT wndProc(MainWindow& mw, UINT msg, WPARAM wParam, LPARAM lParam) override;
};