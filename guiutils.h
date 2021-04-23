#pragma once
#include "window.h"
#include "d2d.h"
#include <windowsx.h>

struct Placement
{
	float x;
	float y;
	float w;
	float h;

	operator D2D1_RECT_F() const noexcept
	{
		return D2D1::RectF(x, y, x + w, y + h);
	}
};

bool pressInPlace(float x, float y, Placement place);