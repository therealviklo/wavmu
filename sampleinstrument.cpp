#include "sampleinstrument.h"

SampleInstrument::SampleInstrument(const char* file) :
	wave(decodeFile((getInstrumentsFolder() / file).string().c_str())),
	tone(60),
	name(file)
{
	envelope = {0.1, 0.1, 0.75, 0.1};
}
	
Sample SampleInstrument::at(double pos, uint16_t channel, Tone tone) const
{
	return wave.at(pos, channel, pow(2.0, (tone - this->tone) / 12.0));
}

std::string SampleInstrument::getName() const
{
	return name;
}