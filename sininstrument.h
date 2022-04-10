#pragma once
#include "instrument.h"

class SinInstrument : public Instrument
{
public:
	SinInstrument();

	Sample at(double pos, uint16_t channel, Tone tone) const override;

	const char* getName() const override;
};