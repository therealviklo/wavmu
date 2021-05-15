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

struct Offset
{
	float x;
	float y;

	Offset onlyX() const noexcept
	{
		return Offset{x, 0.0f};
	}

	Offset onlyY() const noexcept
	{
		return Offset{0.0f, y};
	}
};

inline Placement operator+(const Placement& place, const Offset& off) noexcept
{
	return {place.x + off.x, place.y + off.y, place.w, place.h};
}

inline D2D1_RECT_F operator+(const D2D1_RECT_F& rect, const Offset& off)
{
	return D2D1::RectF(
		rect.left + off.x,
		rect.top + off.y,
		rect.right + off.x,
		rect.bottom + off.y
	);
}