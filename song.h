#pragma once
#include <cstdint>

typedef uint8_t Tone;

struct Note
{
	double timestamp;
	double length;
	Tone tone;
};