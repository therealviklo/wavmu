#pragma once
#include "winerror.h"
#include "instrument.h"
#include "fs.h"

class SampleInstrument : public Instrument
{
private:
	Wave wave;
	Tone tone;
	std::string name;
public:
	SampleInstrument(const char* file);
	
	Sample at(double pos, uint16_t channel, Tone tone) const override;

	const char* getName() const override;
};