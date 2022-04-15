#include "sininstrument.h"

const SinInstrument defInstr;
const Instrument& defaultInstrument = defInstr;

SinInstrument::SinInstrument()
{
	envelope = {0.01, 0.01, 0.75, 0.01};
}

Sample SinInstrument::at(double pos, uint16_t /*channel*/, Tone tone) const
{
	return sin(pos * 2.0 * std::numbers::pi * 440.0 * pow(2.0, (tone - 133.0) / 12.0)) * ((int32_t)(~(uint32_t)0 >> 1) / 10);
}

const char* SinInstrument::getName() const
{
	return "";
}