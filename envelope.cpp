#include "envelope.h"

double Envelope::at(double time, double noteDuration) const
{
	if (time <= attack)
		return std::lerp(0.0, 1.0, 	time / attack);
	else if (time <= attack + decay)
		return std::lerp(1.0, sustain, (time - attack) / decay);
	else if (time <= noteDuration)
		return sustain;
	else
		return std::lerp(sustain, 0.0, (time - noteDuration) / release);
}