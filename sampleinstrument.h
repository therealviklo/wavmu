#pragma once
#include <filesystem>
#include "instrument.h"

class SampleInstrument : public Instrument
{
private:
	Wave wave;
	Tone tone;
	std::string name;
public:
	SampleInstrument(const char* file);
	
	Sample at(double pos, uint16_t channel, Tone tone) const override;

	std::string getName() const override;
};