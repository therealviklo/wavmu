#pragma once
#include "instrument.h"

class FakeInstrument : public Instrument
{
private:
	std::string name;
public:
	FakeInstrument(std::string name);

	Sample at(double pos, uint16_t channel, Tone tone) const override;

	std::string getName() const override;
};