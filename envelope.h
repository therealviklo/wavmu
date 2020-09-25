#pragma once
#include <cmath>

struct Envelope
{
	double attack;
	double decay;
	double sustain;
	double release;

	double at(double time, double noteDuration) const;
};
