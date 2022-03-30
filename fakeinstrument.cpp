#include "fakeinstrument.h"

FakeInstrument::FakeInstrument(std::string name) :
	name(name) {}

Sample FakeInstrument::at(double pos, uint16_t channel, Tone tone) const
{
	return defaultInstrument.at(pos, channel, tone);
}

std::string FakeInstrument::getName() const
{
	return name;
}