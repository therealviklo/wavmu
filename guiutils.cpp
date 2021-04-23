#include "guiutils.h"

bool pressInPlace(float x, float y, Placement place)
{
	return x > place.x
		&& x < place.x + place.w
		&& y > place.y
		&& y < place.y + place.h;
}