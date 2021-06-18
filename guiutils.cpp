#include "guiutils.h"

bool pressInPlace(float x, float y, Placement place)
{
	return x > place.x
		&& x < place.x + place.w
		&& y > place.y
		&& y < place.y + place.h;
}

bool rectsColliding(D2D1_RECT_F a, D2D1_RECT_F b)
{
	return a.left <= b.right
		&& b.left <= a.right
		&& a.top <= b.bottom
		&& b.top <= a.bottom;
}