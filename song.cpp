#include "song.h"

double Section::calcLength()
{
	double longestYet = 0.0;
	for (const auto& note : notes)
	{
		longestYet = std::max<double>(note.timestamp + note.length, longestYet);
	}
	return longestYet;
}