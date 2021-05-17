#include "instrument.h"

SampleInstrument::SampleInstrument(const char* file)
	: wave(file),
	  tone(60)
{
	envelope = {0.1, 0.1, 0.75, 0.1};
}
	
Sample SampleInstrument::at(double pos, uint16_t channel, Tone tone) const
{
	return wave.at(pos, channel, pow(2.0, (tone - this->tone) / 12.0));
}

SinInstrument::SinInstrument()
{
	envelope = {0.01, 0.01, 0.75, 0.01};
}

Sample SinInstrument::at(double pos, uint16_t /*channel*/, Tone tone) const
{
	return sin(pos * 2.0 * std::numbers::pi * 440.0 * pow(2.0, (tone - 5.0) / 12.0)) * ((int32_t)(~(uint32_t)0 >> 1) / 2);
}